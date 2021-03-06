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

#include "moduleswindow.h"

#include "debuggeractions.h"
#include "debuggerconstants.h"
#include "debuggercore.h"
#include "debuggerengine.h"

#include <utils/qtcassert.h>
#include <utils/savedaction.h>

#include <QtCore/QDebug>
#include <QtCore/QProcess>

#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QResizeEvent>


///////////////////////////////////////////////////////////////////////////
//
// ModulesWindow
//
///////////////////////////////////////////////////////////////////////////

namespace Debugger {
namespace Internal {

ModulesWindow::ModulesWindow(QWidget *parent)
  : BaseWindow(parent)
{
    setWindowTitle(tr("Modules"));
    setAlwaysAdjustColumnsAction(debuggerCore()->action(AlwaysAdjustModulesColumnWidths));

    connect(this, SIGNAL(activated(QModelIndex)),
        SLOT(moduleActivated(QModelIndex)));
}

void ModulesWindow::moduleActivated(const QModelIndex &index)
{
    DebuggerEngine *engine = debuggerCore()->currentEngine();
    QTC_ASSERT(engine, return);
    engine->gotoLocation(index.data().toString());
}

void ModulesWindow::contextMenuEvent(QContextMenuEvent *ev)
{
    QString name;
    QString fileName;
    QModelIndex index = indexAt(ev->pos());
    if (index.isValid())
        index = index.sibling(index.row(), 0);
    if (index.isValid()) {
        name = index.data().toString();
        fileName = index.sibling(index.row(), 1).data().toString();
    }

    DebuggerEngine *engine = debuggerCore()->currentEngine();
    QTC_ASSERT(engine, return);
    const bool enabled = engine->debuggerActionsEnabled();
    const unsigned capabilities = engine->debuggerCapabilities();

    QMenu menu;

    QAction *actUpdateModuleList
        = new QAction(tr("Update Module List"), &menu);
    actUpdateModuleList
        ->setEnabled(enabled && (capabilities & ReloadModuleCapability));

    QAction *actShowModuleSources
        = new QAction(tr("Show Source Files for Module \"%1\"").arg(name), &menu);
    actShowModuleSources
        ->setEnabled(enabled && (capabilities & ReloadModuleCapability));

    QAction *actLoadSymbolsForAllModules
        = new QAction(tr("Load Symbols for All Modules"), &menu);
    actLoadSymbolsForAllModules
        -> setEnabled(enabled && (capabilities & ReloadModuleSymbolsCapability));

    QAction *actExamineAllModules
        = new QAction(tr("Examine All Modules"), &menu);
    actExamineAllModules
        -> setEnabled(enabled && (capabilities & ReloadModuleSymbolsCapability));

    QAction *actLoadSymbolsForModule = 0;
    QAction *actEditFile = 0;
    QAction *actShowModuleSymbols = 0;
    QAction *actShowDependencies = 0; // Show dependencies by running 'depends.exe'
    if (name.isEmpty()) {
        actLoadSymbolsForModule = new QAction(tr("Load Symbols for Module"), &menu);
        actLoadSymbolsForModule->setEnabled(false);
        actEditFile = new QAction(tr("Edit File"), &menu);
        actEditFile->setEnabled(false);
        actShowModuleSymbols = new QAction(tr("Show Symbols"), &menu);
        actShowModuleSymbols->setEnabled(false);
        actShowDependencies = new QAction(tr("Show Dependencies"), &menu);
        actShowDependencies->setEnabled(false);
    } else {
        actLoadSymbolsForModule
            = new QAction(tr("Load Symbols for Module \"%1\"").arg(name), &menu);
        actLoadSymbolsForModule
            ->setEnabled(capabilities & ReloadModuleSymbolsCapability);
        actEditFile
            = new QAction(tr("Edit File \"%1\"").arg(name), &menu);
        actShowModuleSymbols
            = new QAction(tr("Show Symbols in File \"%1\"").arg(name), &menu);
        actShowModuleSymbols
            ->setEnabled(capabilities & ShowModuleSymbolsCapability);
        actShowDependencies = new QAction(tr("Show Dependencies of \"%1\"").arg(name), &menu);
        actShowDependencies->setEnabled(!fileName.isEmpty());
#ifndef Q_OS_WIN
        // FIXME: Dependencies only available on Windows, when "depends" is installed.
        actShowDependencies->setEnabled(false);
#endif
    }

    menu.addAction(actUpdateModuleList);
    //menu.addAction(actShowModuleSources);  // FIXME
    menu.addAction(actShowDependencies);
    menu.addAction(actLoadSymbolsForAllModules);
    menu.addAction(actExamineAllModules);
    menu.addAction(actLoadSymbolsForModule);
    menu.addAction(actEditFile);
    menu.addAction(actShowModuleSymbols);
    addBaseContextActions(&menu);

    QAction *act = menu.exec(ev->globalPos());

    if (act == actUpdateModuleList)
        engine->reloadModules();
    else if (act == actShowModuleSources)
        engine->loadSymbols(name);
    else if (act == actLoadSymbolsForAllModules)
        engine->loadAllSymbols();
    else if (act == actExamineAllModules)
        engine->examineModules();
    else if (act == actLoadSymbolsForModule)
        engine->loadSymbols(name);
    else if (act == actEditFile)
        engine->gotoLocation(name);
    else if (act == actShowModuleSymbols)
        engine->requestModuleSymbols(name);
    else if (actShowDependencies && act == actShowDependencies)
        QProcess::startDetached(QLatin1String("depends"), QStringList(fileName));
    else
        handleBaseContextAction(act);
}

} // namespace Internal
} // namespace Debugger
