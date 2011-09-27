#include "pythonprojectrunconfigurationwidget.h"
#include "pythonprojectrunconfiguration.h"
#include "pythonprojecttarget.h"
#include "pythonproject.h"

#include <coreplugin/helpmanager.h>
#include <coreplugin/icore.h>
#include <projectexplorer/environmentwidget.h>
#include <projectexplorer/projectexplorer.h>
#include <utils/debuggerlanguagechooser.h>
#include <utils/detailswidget.h>
#include <utils/environment.h>
#include <utils/qtcassert.h>
#include <qtsupport/qtsupportconstants.h>
#include <qtsupport/qtversionmanager.h>

#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>

using Core::ICore;
using Utils::DebuggerLanguageChooser;
using QtSupport::QtVersionManager;

namespace PythonProjectManager {
namespace Internal {

PythonProjectRunConfigurationWidget::PythonProjectRunConfigurationWidget(PythonProjectRunConfiguration *rc) :
    m_runConfiguration(rc),
    m_fileListCombo(0),
    m_fileListModel(new QStandardItemModel(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    //
    // Arguments
    //

    Utils::DetailsWidget *detailsWidget = new Utils::DetailsWidget();
    detailsWidget->setState(Utils::DetailsWidget::NoSummary);

    QWidget *formWidget = new QWidget(detailsWidget);
    detailsWidget->setWidget(formWidget);
    QFormLayout *form = new QFormLayout(formWidget);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_fileListCombo = new QComboBox;
    m_fileListCombo->setModel(m_fileListModel);

    connect(m_fileListCombo, SIGNAL(activated(int)), this, SLOT(setMainScript(int)));
    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(fileListChanged()),
            SLOT(updateFileComboBox()));

    QLineEdit *pythonArgs = new QLineEdit;
    pythonArgs->setText(rc->m_pythonArgs);
    connect(pythonArgs, SIGNAL(textChanged(QString)), this, SLOT(onPythonArgsChanged()));

    form->addRow(tr("Arguments:"), pythonArgs);
    form->addRow(tr("Main PYTHON file:"), m_fileListCombo);

    layout->addWidget(detailsWidget);

    //
    // Environment
    //

    QLabel *environmentLabel = new QLabel(this);
    environmentLabel->setText(tr("Run Environment"));
    QFont f = environmentLabel->font();
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() *1.2);
    environmentLabel->setFont(f);

    layout->addWidget(environmentLabel);

    QWidget *baseEnvironmentWidget = new QWidget;
    QHBoxLayout *baseEnvironmentLayout = new QHBoxLayout(baseEnvironmentWidget);
    baseEnvironmentLayout->setMargin(0);
    m_environmentWidget = new ProjectExplorer::EnvironmentWidget(this, baseEnvironmentWidget);
    m_environmentWidget->setBaseEnvironment(rc->baseEnvironment());
    m_environmentWidget->setBaseEnvironmentText(tr("System Environment"));
    m_environmentWidget->setUserChanges(rc->userEnvironmentChanges());

    connect(m_environmentWidget, SIGNAL(userChangesChanged()),
            this, SLOT(userChangesChanged()));

    layout->addWidget(m_environmentWidget);

    updateFileComboBox();
}

static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

void PythonProjectRunConfigurationWidget::updateFileComboBox()
{
    PythonProject *project = m_runConfiguration->pythonTarget()->pythonProject();
    QDir projectDir = project->projectDir();

    m_fileListModel->clear();
    m_fileListModel->appendRow(new QStandardItem(CURRENT_FILE));
    QModelIndex currentIndex;
    QModelIndex fileInPythonProjectIndex;

    const QString mainScriptInFilePath = projectDir.absoluteFilePath(project->mainFile());

    QStringList sortedFiles = project->files();
    if (!sortedFiles.contains(mainScriptInFilePath))
        sortedFiles += mainScriptInFilePath;

    // make paths relative to project directory
    QStringList relativeFiles;
    foreach (const QString &fn, sortedFiles) {
        relativeFiles += projectDir.relativeFilePath(fn);
    }
    sortedFiles = relativeFiles;

    qStableSort(sortedFiles.begin(), sortedFiles.end(), caseInsensitiveLessThan);

    QString mainScriptPath;
    if (m_runConfiguration->mainScriptSource() != PythonProjectRunConfiguration::FileInEditor)
        mainScriptPath = projectDir.relativeFilePath(m_runConfiguration->mainScript());

    foreach (const QString &fn, sortedFiles) {
        QFileInfo fileInfo(fn);
        if (fileInfo.suffix() != QLatin1String("python"))
            continue;

        QStandardItem *item = new QStandardItem(fn);
        m_fileListModel->appendRow(item);

        if (mainScriptPath == fn)
            currentIndex = item->index();

        if (mainScriptInFilePath == fn)
            fileInPythonProjectIndex = item->index();
    }

    if (currentIndex.isValid()) {
        m_fileListCombo->setCurrentIndex(currentIndex.row());
    } else {
        m_fileListCombo->setCurrentIndex(0);
    }

    if (fileInPythonProjectIndex.isValid()) {
        QFont font;
        font.setBold(true);
        m_fileListModel->setData(fileInPythonProjectIndex, font, Qt::FontRole);
    }
}

void PythonProjectRunConfigurationWidget::setMainScript(int index)
{
    PythonProject *project = m_runConfiguration->pythonTarget()->pythonProject();
    QDir projectDir = project->projectDir();
    if (index == 0) {
        m_runConfiguration->setScriptSource(PythonProjectRunConfiguration::FileInEditor);
    } else {
        const QString path = m_fileListModel->data(m_fileListModel->index(index, 0)).toString();
        if (projectDir.relativeFilePath(project->mainFile()) == path) {
            m_runConfiguration->setScriptSource(PythonProjectRunConfiguration::FileInProjectFile);
        } else {
            m_runConfiguration->setScriptSource(PythonProjectRunConfiguration::FileInSettings, path);
        }
    }
}

void PythonProjectRunConfigurationWidget::onPythonArgsChanged()
{
    if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender()))
        m_runConfiguration->m_pythonArgs = lineEdit->text();
}

void PythonProjectRunConfigurationWidget::userChangesChanged()
{
    m_runConfiguration->setUserEnvironmentChanges(m_environmentWidget->userChanges());
}

void PythonProjectRunConfigurationWidget::userEnvironmentChangesChanged()
{
    m_environmentWidget->setUserChanges(m_runConfiguration->userEnvironmentChanges());
}


} // namespace Internal
} // namespace PythonProjectManager
