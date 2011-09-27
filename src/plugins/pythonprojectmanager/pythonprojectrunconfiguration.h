#ifndef PYTHONPROJECTRUNCONFIGURATION_H
#define PYTHONPROJECTRUNCONFIGURATION_H

#include "pythonprojectmanager_global.h"

#include <projectexplorer/runconfiguration.h>

#include <QtCore/QWeakPointer>

QT_FORWARD_DECLARE_CLASS(QStringListModel)

namespace Core {
    class IEditor;
}

namespace Utils {
    class Environment;
    class EnvironmentItem;
}

namespace PythonProjectManager {

namespace Internal {
    class PythonProjectTarget;
    class PythonProjectRunConfigurationFactory;
    class PythonProjectRunConfigurationWidget;
}

class PYTHONPROJECTMANAGER_EXPORT PythonProjectRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    friend class Internal::PythonProjectRunConfigurationFactory;
    friend class Internal::PythonProjectRunConfigurationWidget;

public:
    PythonProjectRunConfiguration(Internal::PythonProjectTarget *parent);
    virtual ~PythonProjectRunConfiguration();

    Internal::PythonProjectTarget *pythonTarget() const;

    QString pythonPath() const;
    QString pythonArguments() const;
    QString workingDirectory() const;

    enum MainScriptSource {
        FileInEditor,
        FileInProjectFile,
        FileInSettings
    };
    MainScriptSource mainScriptSource() const;
    void setScriptSource(MainScriptSource source, const QString &settingsPath = QString());

    QString mainScript() const;

    Utils::Environment environment() const;

    // RunConfiguration
    bool isEnabled() const;
    virtual QWidget *createConfigurationWidget();
    Utils::OutputFormatter *createOutputFormatter() const;
    QVariantMap toMap() const;

    ProjectExplorer::Abi abi() const;

public slots:
    void changeCurrentFile(Core::IEditor*);

private slots:
    void updateEnabled();

protected:
    PythonProjectRunConfiguration(Internal::PythonProjectTarget *parent,
                               PythonProjectRunConfiguration *source);
    virtual bool fromMap(const QVariantMap &map);
    void setEnabled(bool value);

private:
    void ctor();
    
    static QString canonicalCapsPath(const QString &filePath);

    Utils::Environment baseEnvironment() const;
    void setUserEnvironmentChanges(const QList<Utils::EnvironmentItem> &diff);
    QList<Utils::EnvironmentItem> userEnvironmentChanges() const;

    // absolute path to current file (if being used)
    QString m_currentFileFilename;
    // absolute path to selected main script (if being used)
    QString m_mainScriptFilename;

    QString m_scriptFile;
    QString m_pythonArgs;

    Internal::PythonProjectTarget *m_projectTarget;
    QWeakPointer<Internal::PythonProjectRunConfigurationWidget> m_configurationWidget;

    bool m_usingCurrentFile;
    bool m_isEnabled;

    QList<Utils::EnvironmentItem> m_userEnvironmentChanges;
};

} // namespace PythonProjectManager

#endif // PYTHONPROJECTRUNCONFIGURATION_H
