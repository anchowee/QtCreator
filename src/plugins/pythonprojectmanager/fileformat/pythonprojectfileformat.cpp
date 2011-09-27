/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "pythonprojectfileformat.h"
#include "pythonprojectitem.h"
#include "filefilteritems.h"

#include <qdeclarative.h>

namespace PythonProjectManager {

void PythonProjectFileFormat::registerDeclarativeTypes()
{
    /*
    pythonRegisterType<PythonProjectManager::PythonProjectContentItem>();
    pythonRegisterType<PythonProjectManager::PythonProjectItem>("PythonProject",1,0,"Project");
    pythonRegisterType<PythonProjectManager::PythonProjectItem>("PythonProject",1,1,"Project");

    pythonRegisterType<PythonProjectManager::PythonFileFilterItem>("PythonProject",1,0,"PythonFiles");
    pythonRegisterType<PythonProjectManager::PythonFileFilterItem>("PythonProject",1,1,"PythonFiles");
    pythonRegisterType<PythonProjectManager::JsFileFilterItem>("PythonProject",1,0,"JavaScriptFiles");
    pythonRegisterType<PythonProjectManager::JsFileFilterItem>("PythonProject",1,1,"JavaScriptFiles");
    pythonRegisterType<PythonProjectManager::ImageFileFilterItem>("PythonProject",1,0,"ImageFiles");
    pythonRegisterType<PythonProjectManager::ImageFileFilterItem>("PythonProject",1,1,"ImageFiles");
    pythonRegisterType<PythonProjectManager::CssFileFilterItem>("PythonProject",1,0,"CssFiles");
    pythonRegisterType<PythonProjectManager::CssFileFilterItem>("PythonProject",1,1,"CssFiles");
    pythonRegisterType<PythonProjectManager::OtherFileFilterItem>("PythonProject",1,1,"Files");
    */
}

} // namespace PythonProjectManager
