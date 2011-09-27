#ifndef PYTHONPROJECTRUNCONTROL_H
#define PYTHONPROJECTRUNCONTROL_H

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

namespace PythonProjectManager {

class PythonProjectRunConfiguration;

namespace Internal {

class PythonProjectRunControl : public ProjectExplorer::RunControl
{
    Q_OBJECT
public:
    explicit PythonProjectRunControl(PythonProjectRunConfiguration *runConfiguration, QString mode);
    virtual ~PythonProjectRunControl ();

    // RunControl
    virtual void start();
    virtual StopResult stop();
    virtual bool isRunning() const;
    virtual QIcon icon() const;

    QString mainPyFile() const;

private slots:
    void processExited(int exitCode);
    void slotBringApplicationToForeground(qint64 pid);
    void slotAppendMessage(const QString &line, Utils::OutputFormat);

private:
    ProjectExplorer::ApplicationLauncher m_applicationLauncher;

    QString m_executable;
    QString m_commandLineArguments;
    QString m_mainPyFile;
};

class PythonProjectRunControlFactory : public ProjectExplorer::IRunControlFactory {
    Q_OBJECT
public:
    explicit PythonProjectRunControlFactory(QObject *parent = 0);
    virtual ~PythonProjectRunControlFactory();

    // IRunControlFactory
    virtual bool canRun(ProjectExplorer::RunConfiguration *runConfiguration, const QString &mode) const;
    virtual ProjectExplorer::RunControl *create(ProjectExplorer::RunConfiguration *runConfiguration, const QString &mode);
    virtual QString displayName() const;
    virtual ProjectExplorer::RunConfigWidget *createConfigurationWidget(ProjectExplorer::RunConfiguration
                                                                        *runConfiguration);
};

} // namespace Internal
} // namespace PythonProjectManager

#endif // PYTHONPROJECTRUNCONTROL_H
