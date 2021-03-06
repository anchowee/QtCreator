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

#include "snapshotwindow.h"
#include "snapshothandler.h"

#include "debuggeractions.h"
#include "debuggerinternalconstants.h"
#include "debuggercore.h"
#include "debuggerengine.h"

#include <utils/qtcassert.h>
#include <utils/savedaction.h>

#include <QtCore/QDebug>

#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QKeyEvent>

namespace Debugger {
namespace Internal {

///////////////////////////////////////////////////////////////////////
//
// SnapshotWindow
//
///////////////////////////////////////////////////////////////////////

SnapshotWindow::SnapshotWindow(SnapshotHandler *handler)
{
    m_snapshotHandler = handler;
    setWindowTitle(tr("Snapshots"));
    setAlwaysAdjustColumnsAction(debuggerCore()->action(AlwaysAdjustSnapshotsColumnWidths));
}

void SnapshotWindow::rowActivated(const QModelIndex &index)
{
    m_snapshotHandler->activateSnapshot(index.row());
}

void SnapshotWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Delete) {
        QItemSelectionModel *sm = selectionModel();
        QTC_ASSERT(sm, return);
        QModelIndexList si = sm->selectedIndexes();
        if (si.isEmpty())
            si.append(currentIndex().sibling(currentIndex().row(), 0));

        foreach (const QModelIndex &idx, si)
            if (idx.column() == 0)
                removeSnapshot(idx.row());
    }
    QTreeView::keyPressEvent(ev);
}

void SnapshotWindow::contextMenuEvent(QContextMenuEvent *ev)
{
    QModelIndex idx = indexAt(ev->pos());

    QMenu menu;

    QAction *actCreate = menu.addAction(tr("Create Snapshot"));
    actCreate->setEnabled(idx.data(SnapshotCapabilityRole).toBool());
    menu.addSeparator();

    QAction *actRemove = menu.addAction(tr("Remove Snapshot"));
    actRemove->setEnabled(idx.isValid());
    
    addBaseContextActions(&menu);

    QAction *act = menu.exec(ev->globalPos());

    if (act == actCreate)
        m_snapshotHandler->createSnapshot(idx.row());
    else if (act == actRemove)
        removeSnapshot(idx.row());
    else
        handleBaseContextAction(act);
}

void SnapshotWindow::removeSnapshot(int i)
{
    m_snapshotHandler->at(i)->quitDebugger();
}

} // namespace Internal
} // namespace Debugger
