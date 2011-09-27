#include "pythonprojectruncontrol.h"
#include "pythonprojectrunconfiguration.h"
#include <coreplugin/icore.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <utils/qtcassert.h>

#include <debugger/debuggerrunner.h>
#include <debugger/debuggerplugin.h>
#include <debugger/debuggerconstants.h>
#include <debugger/debuggerstartparameters.h>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qmlobservertool.h>

#include <pythonprojectmanager/pythonprojectplugin.h>

using namespace ProjectExplorer;

namespace PythonProjectManager {

namespace Internal {

PythonProjectRunControl::PythonProjectRunControl(PythonProjectRunConfiguration *runConfiguration, QString mode)
    : RunControl(runConfiguration, mode)
{
    qDebug() << __func__;
    m_applicationLauncher.setEnvironment(runConfiguration->environment());
    m_applicationLauncher.setWorkingDirectory(runConfiguration->workingDirectory());

    m_executable = runConfiguration->pythonPath();
    qDebug() << "executable: " << m_executable;
    
    m_mainPyFile = runConfiguration->mainScript();
    qDebug() << "mainPyFile: " << m_mainPyFile;
    m_commandLineArguments.append(m_mainPyFile);
    qDebug() << "commandLineArguments: " << m_commandLineArguments;

    connect(&m_applicationLauncher, SIGNAL(appendMessage(QString,Utils::OutputFormat)),
            this, SLOT(slotAppendMessage(QString, Utils::OutputFormat)));
    connect(&m_applicationLauncher, SIGNAL(processExited(int)),
            this, SLOT(processExited(int)));
    connect(&m_applicationLauncher, SIGNAL(bringToForegroundRequested(qint64)),
            this, SLOT(slotBringApplicationToForeground(qint64)));
}

PythonProjectRunControl::~PythonProjectRunControl()
{
    stop();
}

void PythonProjectRunControl::start()
{
    m_applicationLauncher.start(ApplicationLauncher::Gui, m_executable,
                                m_commandLineArguments);
    setApplicationProcessHandle(ProcessHandle(m_applicationLauncher.applicationPID()));
    emit started();
    QString msg = tr("Starting %1 %2\n")
        .arg(QDir::toNativeSeparators(m_executable), m_commandLineArguments);
    appendMessage(msg, Utils::NormalMessageFormat);
}

RunControl::StopResult PythonProjectRunControl::stop()
{
    m_applicationLauncher.stop();
    return StoppedSynchronously;
}

bool PythonProjectRunControl::isRunning() const
{
    return m_applicationLauncher.isRunning();
}

QIcon PythonProjectRunControl::icon() const
{
    return QIcon(ProjectExplorer::Constants::ICON_RUN_SMALL);
}

void PythonProjectRunControl::slotBringApplicationToForeground(qint64 pid)
{
    bringApplicationToForeground(pid);
}

void PythonProjectRunControl::slotAppendMessage(const QString &line, Utils::OutputFormat format)
{
    appendMessage(line, format);
}

void PythonProjectRunControl::processExited(int exitCode)
{
    QString msg = tr("%1 exited with code %2\n")
        .arg(QDir::toNativeSeparators(m_executable)).arg(exitCode);
    appendMessage(msg, exitCode ? Utils::ErrorMessageFormat : Utils::NormalMessageFormat);
    emit finished();
}

QString PythonProjectRunControl::mainPyFile() const
{
    return m_mainPyFile;
}

PythonProjectRunControlFactory::PythonProjectRunControlFactory(QObject *parent)
    : IRunControlFactory(parent)
{
}

PythonProjectRunControlFactory::~PythonProjectRunControlFactory()
{
}

bool PythonProjectRunControlFactory::canRun(RunConfiguration *runConfiguration,
                                  const QString &mode) const
{
    PythonProjectRunConfiguration *config =
        qobject_cast<PythonProjectRunConfiguration*>(runConfiguration);
    if (!config)
        return false;
    if (mode == ProjectExplorer::Constants::RUNMODE)
        return !config->pythonPath().isEmpty();

    return false;
}

RunControl *PythonProjectRunControlFactory::create(RunConfiguration *runConfiguration,
                                         const QString &mode)
{
    QTC_ASSERT(canRun(runConfiguration, mode), return 0);
    PythonProjectRunConfiguration *config = qobject_cast<PythonProjectRunConfiguration *>(runConfiguration);

    QList<ProjectExplorer::RunControl *> runcontrols =
            ProjectExplorer::ProjectExplorerPlugin::instance()->runControls();
    foreach (ProjectExplorer::RunControl *rc, runcontrols) {
        if (PythonProjectRunControl *qrc = qobject_cast<PythonProjectRunControl *>(rc)) {
            if (qrc->mainPyFile() == config->mainScript())
                // Asking the user defeats the purpose
                // Making it configureable might be worth it
                qrc->stop();
        }
    }

    RunControl *runControl = 0;
    if (mode == ProjectExplorer::Constants::RUNMODE)
        runControl = new PythonProjectRunControl(config, mode);

    return runControl;
}

QString PythonProjectRunControlFactory::displayName() const
{
    return tr("Run");
}

ProjectExplorer::RunConfigWidget *PythonProjectRunControlFactory::createConfigurationWidget(RunConfiguration *runConfiguration)
{
    Q_UNUSED(runConfiguration)
    return 0;
}

} // namespace Internal
} // namespace PythonProjectManager
