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

#ifndef TASKHUB_H
#define TASKHUB_H

#include "task.h"

#include <QtCore/QObject>
#include <QtGui/QIcon>

namespace ProjectExplorer {
class Task;

class PROJECTEXPLORER_EXPORT TaskHub : public QObject
{
    Q_OBJECT
public:
    TaskHub();
    virtual ~TaskHub();

    void addCategory(const QString &categoryId, const QString &displayName, bool visible = true);
    void addTask(const Task &task);
    void clearTasks(const QString &categoryId = QString());
    void removeTask(const Task &task);
    void setCategoryVisibility(const QString &categoryId, bool visible);

    void popup(bool withFocus);

    // TODO now there are two places for icons
    QIcon taskTypeIcon(ProjectExplorer::Task::TaskType t) const;
signals:
    void categoryAdded(const QString &categoryId, const QString &displayName, bool visible);
    void taskAdded(const ProjectExplorer::Task &task);
    void taskRemoved(const ProjectExplorer::Task &task);
    void tasksCleared(const QString &categoryId);
    void categoryVisibilityChanged(const QString &categoryId, bool visible);
    void popupRequested(bool withFocus);
private:
    const QIcon m_errorIcon;
    const QIcon m_warningIcon;
};
} // namespace ProjectExplorer
#endif // TASKHUB_H
