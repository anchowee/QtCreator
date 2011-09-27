#ifndef PYTHONPROJECTTARGET_H
#define PYTHONPROJECTTARGET_H

#include <projectexplorer/target.h>

#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

namespace PythonProjectManager {
class PythonProject;
class PythonProjectRunConfiguration;

namespace Internal {

class PythonProjectTargetFactory;

class PythonProjectTarget : public ProjectExplorer::Target
{
    Q_OBJECT
    friend class PythonProjectTargetFactory;

public:
    explicit PythonProjectTarget(PythonProject *parent);
    ~PythonProjectTarget();

    ProjectExplorer::BuildConfigWidget *createConfigWidget();

    PythonProject *pythonProject() const;

    ProjectExplorer::IBuildConfigurationFactory *buildConfigurationFactory() const;

protected:
    bool fromMap(const QVariantMap &map);
};

class PythonProjectTargetFactory : public ProjectExplorer::ITargetFactory
{
    Q_OBJECT

public:
    explicit PythonProjectTargetFactory(QObject *parent = 0);
    ~PythonProjectTargetFactory();

    bool supportsTargetId(const QString &id) const;
    QStringList supportedTargetIds(ProjectExplorer::Project *parent) const;
    QString displayNameForId(const QString &id) const;

    bool canCreate(ProjectExplorer::Project *parent, const QString &id) const;
    PythonProjectTarget *create(ProjectExplorer::Project *parent, const QString &id);
    bool canRestore(ProjectExplorer::Project *parent, const QVariantMap &map) const;
    PythonProjectTarget *restore(ProjectExplorer::Project *parent, const QVariantMap &map);
};

} // namespace Internal
} // namespace PythonProjectManager

#endif // PYTHONPROJECTTARGET_H
