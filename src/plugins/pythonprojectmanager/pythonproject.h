#ifndef PYTHONPROJECT_H
#define PYTHONPROJECT_H

#include "pythonprojectmanager_global.h"

#include <projectexplorer/project.h>

#include <QtDeclarative/QDeclarativeEngine>

namespace QmlJS {
class ModelManagerInterface;
}

namespace Utils {
class FileSystemWatcher;
}

namespace PythonProjectManager {

class PythonProjectItem;

namespace Internal {
class Manager;
class PythonProjectFile;
class PythonProjectTarget;
class PythonProjectNode;
} // namespace Internal

class PYTHONPROJECTMANAGER_EXPORT PythonProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    PythonProject(Internal::Manager *manager, const QString &filename);
    virtual ~PythonProject();

    QString filesFileName() const;

    QString displayName() const;
    QString id() const;
    Core::IFile *file() const;
    ProjectExplorer::IProjectManager *projectManager() const;
    Internal::PythonProjectTarget *activeTarget() const;

    QList<ProjectExplorer::Project *> dependsOn();

    QList<ProjectExplorer::BuildConfigWidget*> subConfigWidgets();

    ProjectExplorer::ProjectNode *rootProjectNode() const;
    QStringList files(FilesMode fileMode) const;

    bool validProjectFile() const;

    enum RefreshOption {
        ProjectFile   = 0x01,
        Files         = 0x02,
        Configuration = 0x04,
        Everything    = ProjectFile | Files | Configuration
    };
    Q_DECLARE_FLAGS(RefreshOptions,RefreshOption)

    void refresh(RefreshOptions options);

    QDir projectDir() const;
    QStringList files() const;
    QString mainFile() const;
    QStringList importPaths() const;

    bool addFiles(const QStringList &filePaths);

private slots:
    void refreshProjectFile();
    void refreshFiles(const QSet<QString> &added, const QSet<QString> &removed);

protected:
    bool fromMap(const QVariantMap &map);

private:
    // plain format
    void parseProject(RefreshOptions options);
    QStringList convertToAbsoluteFiles(const QStringList &paths) const;

    Internal::Manager *m_manager;
    QString m_fileName;
    Internal::PythonProjectFile *m_file;
    QString m_projectName;
    QmlJS::ModelManagerInterface *m_modelManager;

    // plain format
    QStringList m_files;

    // python based, new format
    QDeclarativeEngine m_engine;
    QWeakPointer<PythonProjectItem> m_projectItem;
    Utils::FileSystemWatcher *m_fileWatcher;

    Internal::PythonProjectNode *m_rootNode;
};

} // namespace PythonProjectManager

Q_DECLARE_OPERATORS_FOR_FLAGS(PythonProjectManager::PythonProject::RefreshOptions)

#endif // PYTHONPROJECT_H
