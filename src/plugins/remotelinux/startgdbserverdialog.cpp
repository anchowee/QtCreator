#include "startgdbserverdialog.h"
#include "ui_startgdbserverdialog.h"

#include "remotelinuxprocesslist.h"
#include "linuxdeviceconfiguration.h"
#include "linuxdeviceconfigurations.h"
#include "remotelinuxusedportsgatherer.h"
#include "portlist.h"

#include <utils/ssh/sshconnection.h>
#include <utils/ssh/sshremoteprocessrunner.h>
#include <utils/qtcassert.h>

#include <QtGui/QMessageBox>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QComboBox>

namespace RemoteLinux {
namespace Internal {

class StartGdbServerDialogPrivate
{
public:
    StartGdbServerDialogPrivate()
        : processList(0)
    {}


    LinuxDeviceConfiguration::ConstPtr currentDevice() const
    {
        LinuxDeviceConfigurations *devices = LinuxDeviceConfigurations::instance();
        return devices->deviceAt(ui.deviceComboBox->currentIndex());
    }

    AbstractRemoteLinuxProcessList *processList;
    QSortFilterProxyModel proxyModel;
    Ui::StartGdbServerDialog ui;
    RemoteLinuxUsedPortsGatherer gatherer;
    Utils::SshRemoteProcessRunner::Ptr runner;
};

} // namespace Internal

StartGdbServerDialog::StartGdbServerDialog(QWidget *parent) :
    QDialog(parent),
    d(new Internal::StartGdbServerDialogPrivate)
{
    LinuxDeviceConfigurations *devices = LinuxDeviceConfigurations::instance();
    d->ui.setupUi(this);
    d->ui.deviceComboBox->setModel(devices);
    connect(&d->gatherer, SIGNAL(error(QString)), SLOT(portGathererError(QString)));
    connect(&d->gatherer, SIGNAL(portListReady()), SLOT(portListReady()));
    if (devices->rowCount() == 0) {
        d->ui.tableView->setEnabled(false);
    } else {
        d->ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        d->proxyModel.setDynamicSortFilter(true);
        d->proxyModel.setFilterKeyColumn(1);
        d->ui.tableView->setModel(&d->proxyModel);
        connect(d->ui.processFilterLineEdit, SIGNAL(textChanged(QString)),
            &d->proxyModel, SLOT(setFilterRegExp(QString)));

        connect(d->ui.tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleSelectionChanged()));
        connect(d->ui.updateListButton, SIGNAL(clicked()),
            SLOT(updateProcessList()));
        connect(d->ui.attachProcessButton, SIGNAL(clicked()), SLOT(attachToProcess()));
        connect(&d->proxyModel, SIGNAL(layoutChanged()),
            SLOT(handleProcessListUpdated()));
        connect(d->ui.deviceComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(attachToDevice(int)));
        handleSelectionChanged();
        attachToDevice(0);
    }
}

StartGdbServerDialog::~StartGdbServerDialog()
{
    delete d->processList;
    delete d;
}


void StartGdbServerDialog::attachToDevice(int index)
{
    LinuxDeviceConfigurations *devices = LinuxDeviceConfigurations::instance();
    delete d->processList;
    d->processList = new GenericRemoteLinuxProcessList(devices->deviceAt(index));
    d->proxyModel.setSourceModel(d->processList);
    connect(d->processList, SIGNAL(error(QString)),
        SLOT(handleRemoteError(QString)));
    connect(d->processList, SIGNAL(modelReset()),
        SLOT(handleProcessListUpdated()));
    connect(d->processList, SIGNAL(processKilled()),
        SLOT(handleProcessKilled()), Qt::QueuedConnection);
    updateProcessList();
}

void StartGdbServerDialog::handleRemoteError(const QString &errorMsg)
{
    QMessageBox::critical(this, tr("Remote Error"), errorMsg);
    d->ui.updateListButton->setEnabled(true);
    handleSelectionChanged();
}

void StartGdbServerDialog::handleProcessListUpdated()
{
    d->ui.updateListButton->setEnabled(true);
    d->ui.tableView->resizeRowsToContents();
    handleSelectionChanged();
}

void StartGdbServerDialog::updateProcessList()
{
    d->ui.updateListButton->setEnabled(false);
    d->ui.attachProcessButton->setEnabled(false);
    d->processList->update();
}

void StartGdbServerDialog::attachToProcess()
{
    const QModelIndexList &indexes =
            d->ui.tableView->selectionModel()->selectedIndexes();
    if (indexes.empty())
        return;
    d->ui.updateListButton->setEnabled(false);
    d->ui.attachProcessButton->setEnabled(false);

    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    PortList ports = device->freePorts();
    const int port = d->gatherer.getNextFreePort(&ports);
    const int row = d->proxyModel.mapToSource(indexes.first()).row();
    QTC_ASSERT(row >= 0, processAborted(); return);
    const int pid = d->processList->pidAt(row);
    if (port == -1) {
        emit processAborted();
    } else {
        emit pidSelected(pid);
        emit portSelected(pid);
        startGdbServerOnPort(port, pid);
    }
}

void StartGdbServerDialog::handleProcessKilled()
{
    updateProcessList();
}

void StartGdbServerDialog::handleSelectionChanged()
{
    d->ui.attachProcessButton->setEnabled(d->ui.tableView->selectionModel()->hasSelection());
}

void StartGdbServerDialog::portGathererError(const QString &text)
{
    d->ui.textBrowser->append(tr("Could not retrieve list of free ports:"));
    d->ui.textBrowser->append(text);
    emit processAborted();
}

void StartGdbServerDialog::portListReady()
{
    d->ui.updateListButton->setEnabled(true);
    d->ui.attachProcessButton->setEnabled(true);
}

void StartGdbServerDialog::startGdbServer()
{
    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    d->gatherer.start(Utils::SshConnection::create(device->sshParameters()), device);
}

void StartGdbServerDialog::handleConnectionError()
{
    d->ui.textBrowser->append(tr("Connection error: %1")
        .arg(d->runner->connection()->errorString()));
    emit processAborted();
}

void StartGdbServerDialog::handleProcessStarted()
{
    d->ui.textBrowser->append(tr("Starting gdbserver..."));
}

void StartGdbServerDialog::handleProcessOutputAvailable(const QByteArray &ba)
{
    d->ui.textBrowser->append(QString::fromUtf8(ba.trimmed()));
}

void StartGdbServerDialog::handleProcessErrorOutput(const QByteArray &ba)
{
    d->ui.textBrowser->append(QString::fromUtf8(ba.trimmed()));
    // "Attached; pid = 16740"
    // "Listening on port 10000"
    int pos = ba.indexOf("Listening on port");
    if (pos != -1) {
        int port = ba.mid(pos + 18).trimmed().toInt();
        d->ui.textBrowser->append(tr("Port %1 is now accessible.").arg(port));
        emit portOpened(port);
    }
}

void StartGdbServerDialog::handleProcessClosed(int status)
{
    d->ui.textBrowser->append(tr("Process gdbserver finished. Status: %1").arg(status));
}

void StartGdbServerDialog::startGdbServerOnPort(int port, int pid)
{
    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    d->runner = Utils::SshRemoteProcessRunner::create(device->sshParameters());
    connect(d->runner.data(), SIGNAL(connectionError(Utils::SshError)),
            SLOT(handleConnectionError()));
    connect(d->runner.data(), SIGNAL(processStarted()),
            SLOT(handleProcessStarted()));
    connect(d->runner.data(), SIGNAL(processOutputAvailable(QByteArray)),
            SLOT(handleProcessOutputAvailable(QByteArray)));
    connect(d->runner.data(), SIGNAL(processErrorOutputAvailable(QByteArray)),
            SLOT(handleProcessErrorOutput(QByteArray)));
    connect(d->runner.data(), SIGNAL(processClosed(int)),
            SLOT(handleProcessClosed(int)));

    QByteArray cmd = "/usr/bin/gdbserver --attach localhost:"
        + QByteArray::number(port) + " " + QByteArray::number(pid);
    d->runner->run(cmd);
}

} // namespace RemoteLinux
