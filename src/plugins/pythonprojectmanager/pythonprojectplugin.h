#ifndef PYTHONPROJECTPLUGIN_H
#define PYTHONPROJECTPLUGIN_H

#include "pythonprojectmanager_global.h"

#include <extensionsystem/iplugin.h>

namespace PythonProjectManager {

class PYTHONPROJECTMANAGER_EXPORT PythonProjectPlugin: public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    PythonProjectPlugin();
    ~PythonProjectPlugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();
};

} // namespace PythonProject

#endif // PYTHONPROJECTPLUGIN_H
