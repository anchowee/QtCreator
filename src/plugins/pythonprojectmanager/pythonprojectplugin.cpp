#include "pythonprojectplugin.h"
#include "pythonprojectmanager.h"
#include "pythonprojectapplicationwizard.h"
#include "pythonprojectconstants.h"
#include "pythonproject.h"
#include "pythonprojectrunconfigurationfactory.h"
#include "pythonprojectruncontrol.h"
#include "pythonprojecttarget.h"
#include "fileformat/pythonprojectfileformat.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/fileiconprovider.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>

#include <texteditor/texteditoractionhandler.h>

#include <projectexplorer/taskhub.h>

#include <qtsupport/qtsupportconstants.h>

#include <QtCore/QtPlugin>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QDebug>

namespace PythonProjectManager {

PythonProjectPlugin::PythonProjectPlugin()
{
    qDebug() << __func__;
}

PythonProjectPlugin::~PythonProjectPlugin()
{
}

bool PythonProjectPlugin::initialize(const QStringList &, QString *errorMessage)
{
    using namespace Core;

    ICore *core = ICore::instance();
    Core::MimeDatabase *mimeDB = core->mimeDatabase();

    const QLatin1String mimetypesXml(":/pythonproject/PythonProject.mimetypes.xml");

    if (!mimeDB->addMimeTypes(mimetypesXml, errorMessage))
        return false;

    Internal::Manager *manager = new Internal::Manager;

    addAutoReleasedObject(manager);
    addAutoReleasedObject(new Internal::PythonProjectRunConfigurationFactory);
    addAutoReleasedObject(new Internal::PythonProjectRunControlFactory);
    addAutoReleasedObject(new Internal::PythonProjectApplicationWizard);
    addAutoReleasedObject(new Internal::PythonProjectTargetFactory);

    PythonProjectFileFormat::registerDeclarativeTypes();

    Core::FileIconProvider *iconProvider = Core::FileIconProvider::instance();
    iconProvider->registerIconOverlayForSuffix(QIcon(QLatin1String(":/pythonproject/images/pythonproject.png")), "pythonproject");
    return true;
}

void PythonProjectPlugin::extensionsInitialized()
{
}

} // namespace PythonProjectManager

Q_EXPORT_PLUGIN(PythonProjectManager::PythonProjectPlugin)
