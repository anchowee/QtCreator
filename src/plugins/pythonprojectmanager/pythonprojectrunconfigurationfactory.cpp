#include "pythonprojectmanagerconstants.h"
#include "pythonprojectrunconfiguration.h"
#include "pythonprojectrunconfigurationfactory.h"
#include "pythonprojecttarget.h"

#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/runconfiguration.h>

#include <QtCore/QDebug>

namespace PythonProjectManager {
namespace Internal {

PythonProjectRunConfigurationFactory::PythonProjectRunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
    qDebug() << __func__;
}

PythonProjectRunConfigurationFactory::~PythonProjectRunConfigurationFactory()
{
}

QStringList PythonProjectRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    if (!qobject_cast<PythonProjectTarget *>(parent))
        return QStringList();
    return QStringList() << QLatin1String(Constants::PYTHON_RC_ID);
}

QString PythonProjectRunConfigurationFactory::displayNameForId(const QString &id) const
{
    if (id == QLatin1String(Constants::PYTHON_RC_ID))
        return tr("Run PYTHON Script");
    return QString();
}

bool PythonProjectRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const QString &id) const
{
    if (!qobject_cast<PythonProjectTarget *>(parent))
        return false;
    return id == QLatin1String(Constants::PYTHON_RC_ID);
}

ProjectExplorer::RunConfiguration *PythonProjectRunConfigurationFactory::create(ProjectExplorer::Target *parent, const QString &id)
{
    if (!canCreate(parent, id))
        return 0;
    PythonProjectTarget *pythonparent = static_cast<PythonProjectTarget *>(parent);
    return new PythonProjectRunConfiguration(pythonparent);
}

bool PythonProjectRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    QString id = ProjectExplorer::idFromMap(map);
    return canCreate(parent, id);
}

ProjectExplorer::RunConfiguration *PythonProjectRunConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    PythonProjectTarget *pythonparent = static_cast<PythonProjectTarget *>(parent);
    PythonProjectRunConfiguration *rc = new PythonProjectRunConfiguration(pythonparent);
    if (rc->fromMap(map))
        return rc;
    delete rc;
    return 0;
}

bool PythonProjectRunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *PythonProjectRunConfigurationFactory::clone(ProjectExplorer::Target *parent,
                                                                     ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    PythonProjectTarget *pythonparent = static_cast<PythonProjectTarget *>(parent);
    return new PythonProjectRunConfiguration(pythonparent, qobject_cast<PythonProjectRunConfiguration *>(source));
}

} // namespace Internal
} // namespace PythonProjectManager

