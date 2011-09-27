#include "pythonprojecttarget.h"

#include "pythonproject.h"
#include "pythonprojectmanagerconstants.h"
#include "pythonprojectrunconfiguration.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtCore/QDebug>

namespace PythonProjectManager {
namespace Internal {

PythonProjectTarget::PythonProjectTarget(PythonProject *parent) :
    ProjectExplorer::Target(parent, QLatin1String(Constants::PYTHON_VIEWER_TARGET_ID))
{
    qDebug() << __func__;
    setDisplayName(
        QApplication::translate("PythonProjectManager::PythonTarget",
        Constants::PYTHON_VIEWER_TARGET_DISPLAY_NAME,
        "Python target display name"));

    setIcon(qApp->style()->standardIcon(QStyle::SP_ComputerIcon));
}

PythonProjectTarget::~PythonProjectTarget()
{
}

ProjectExplorer::BuildConfigWidget *PythonProjectTarget::createConfigWidget()
{
    qDebug() << __func__;
    return 0;
}

PythonProject *PythonProjectTarget::pythonProject() const
{
    qDebug() << __func__;
    return static_cast<PythonProject *>(project());
}

ProjectExplorer::IBuildConfigurationFactory *PythonProjectTarget::buildConfigurationFactory(void) const
{
    qDebug() << __func__;
    return 0;
}

bool PythonProjectTarget::fromMap(const QVariantMap &map)
{
    qDebug() << __func__;
    if (!Target::fromMap(map))
        return false;

    if (runConfigurations().isEmpty()) {
        qWarning() << "Failed to restore run configuration of Python project!";
        return false;
    }

    setDisplayName(
        QApplication::translate("PythonProjectManager::PythonTarget",
                                Constants::PYTHON_VIEWER_TARGET_DISPLAY_NAME,
                                "Python target display name"));

    return true;
}

PythonProjectTargetFactory::PythonProjectTargetFactory(QObject *parent) :
    ITargetFactory(parent)
{
    qDebug() << __func__;
}

PythonProjectTargetFactory::~PythonProjectTargetFactory()
{
}

bool PythonProjectTargetFactory::supportsTargetId(const QString &id) const
{
    qDebug() << __func__;
    return id == QLatin1String(Constants::PYTHON_VIEWER_TARGET_ID);
}

QStringList PythonProjectTargetFactory::supportedTargetIds(ProjectExplorer::Project *parent) const
{
    qDebug() << __func__;
    if (!qobject_cast<PythonProject *>(parent))
        return QStringList();

    return QStringList() << QLatin1String(Constants::PYTHON_VIEWER_TARGET_ID);
}

QString PythonProjectTargetFactory::displayNameForId(const QString &id) const
{
    qDebug() << __func__;
    if (id == QLatin1String(Constants::PYTHON_VIEWER_TARGET_ID))
        return QCoreApplication::translate(
                            "PythonProjectManager::PythonTarget",
                            Constants::PYTHON_VIEWER_TARGET_DISPLAY_NAME,
                            "Python target display name");
    return QString();
}

bool PythonProjectTargetFactory::canCreate(ProjectExplorer::Project *parent, const QString &id) const
{
    qDebug() << __func__;
    if (!qobject_cast<PythonProject *>(parent))
        return false;
    return id == QLatin1String(Constants::PYTHON_VIEWER_TARGET_ID);
}

PythonProjectTarget *PythonProjectTargetFactory::create(ProjectExplorer::Project *parent, const QString &id)
{
    qDebug() << __func__;
    if (!canCreate(parent, id))
        return 0;
    PythonProject *pythonproject(static_cast<PythonProject *>(parent));
    PythonProjectTarget *target = new PythonProjectTarget(pythonproject);

    // Add RunConfiguration (PYTHON does not have BuildConfigurations)
    PythonProjectRunConfiguration *runConf = new PythonProjectRunConfiguration(target);
    target->addRunConfiguration(runConf);

    return target;
}

bool PythonProjectTargetFactory::canRestore(ProjectExplorer::Project *parent, const QVariantMap &map) const
{
    qDebug() << __func__;
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

PythonProjectTarget *PythonProjectTargetFactory::restore(ProjectExplorer::Project *parent, const QVariantMap &map)
{
    qDebug() << __func__;
    if (!canRestore(parent, map))
        return 0;
    PythonProject *pythonproject(static_cast<PythonProject *>(parent));
    PythonProjectTarget *target(new PythonProjectTarget(pythonproject));
    if (target->fromMap(map))
        return target;
    delete target;
    return 0;
}

} // namespace Internal
} // namespace PythonProjectManager
