TEMPLATE = lib
TARGET = PythonProjectManager

QT += declarative

include(../../qtcreatorplugin.pri)
include(pythonprojectmanager_dependencies.pri)
include(fileformat/fileformat.pri)

DEFINES += PYTHONPROJECTMANAGER_LIBRARY
HEADERS += pythonproject.h \
    pythonprojectplugin.h \
    pythonprojectmanager.h \
    pythonprojectconstants.h \
    pythonprojectnodes.h \
    pythonprojectfile.h \
    pythonprojectruncontrol.h \
    pythonprojectrunconfiguration.h \
    pythonprojectrunconfigurationfactory.h \
    pythonprojectapplicationwizard.h \
    pythonprojectmanager_global.h \
    pythonprojectmanagerconstants.h \
    pythonprojecttarget.h \
    pythonprojectrunconfigurationwidget.h

SOURCES += pythonproject.cpp \
    pythonprojectplugin.cpp \
    pythonprojectmanager.cpp \
    pythonprojectnodes.cpp \
    pythonprojectfile.cpp \
    pythonprojectruncontrol.cpp \
    pythonprojectrunconfiguration.cpp \
    pythonprojectrunconfigurationfactory.cpp \
    pythonprojectapplicationwizard.cpp \
    pythonprojecttarget.cpp \
    pythonprojectrunconfigurationwidget.cpp

RESOURCES += pythonproject.qrc

OTHER_FILES += PythonProject.mimetypes.xml
