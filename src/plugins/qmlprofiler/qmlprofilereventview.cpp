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

#include "qmlprofilereventview.h"

#include <QtCore/QUrl>
#include <QtCore/QHash>

#include <QtGui/QStandardItem>
#include <QtGui/QHeaderView>

#include <QtGui/QApplication>
#include <QtGui/QClipboard>

#include <QtGui/QContextMenuEvent>
#include <QDebug>


using namespace QmlJsDebugClient;

namespace QmlProfiler {
namespace Internal {

////////////////////////////////////////////////////////////////////////////////////

class EventsViewItem : public QStandardItem
{
public:
    EventsViewItem(const QString &text) : QStandardItem(text) {}

    virtual bool operator<(const QStandardItem &other) const
    {
        if (data().type() == QVariant::String) {
            // first column
            if (column() == 0) {
                return data(FilenameRole).toString() == other.data(FilenameRole).toString() ?
                        data(LineRole).toInt() < other.data(LineRole).toInt() :
                        data(FilenameRole).toString() < other.data(FilenameRole).toString();
            } else {
                return data().toString() < other.data().toString();
            }
        }

        return data().toDouble() < other.data().toDouble();
    }
};


////////////////////////////////////////////////////////////////////////////////////

class QmlProfilerEventsView::QmlProfilerEventsViewPrivate
{
public:
    QmlProfilerEventsViewPrivate(QmlProfilerEventsView *qq) : q(qq) {}

    void buildModelFromList(const QmlEventDescriptions &list, QStandardItem *parentItem, const QmlEventDescriptions &visitedFunctionsList = QmlEventDescriptions() );
    void buildV8ModelFromList( const QV8EventDescriptions &list );
    int getFieldCount();
    QString displayTime(double time) const;
    QString nameForType(int typeNumber) const;

    QString textForItem(QStandardItem *item, bool recursive) const;


    QmlProfilerEventsView *q;

    QmlProfilerEventsView::ViewTypes m_viewType;
    QmlProfilerEventList *m_eventStatistics;
    QStandardItemModel *m_model;
    QList<bool> m_fieldShown;
    bool m_showAnonymous;
    int m_firstNumericColumn;
};


////////////////////////////////////////////////////////////////////////////////////

QmlProfilerEventsView::QmlProfilerEventsView(QWidget *parent, QmlProfilerEventList *model) :
    QTreeView(parent), d(new QmlProfilerEventsViewPrivate(this))
{
    setObjectName("QmlProfilerEventsView");
    header()->setResizeMode(QHeaderView::Interactive);
    header()->setDefaultSectionSize(100);
    header()->setMinimumSectionSize(50);
    setSortingEnabled(false);
    setFrameStyle(QFrame::NoFrame);

    d->m_model = new QStandardItemModel(this);
    setModel(d->m_model);
    connect(this,SIGNAL(clicked(QModelIndex)), this,SLOT(jumpToItem(QModelIndex)));

    d->m_eventStatistics = 0;
    setEventStatisticsModel(model);

    d->m_showAnonymous = false;
    d->m_firstNumericColumn = 0;

    // default view
    setViewType(EventsView);
}

QmlProfilerEventsView::~QmlProfilerEventsView()
{
    clear();
    delete d->m_model;
}

void QmlProfilerEventsView::setEventStatisticsModel( QmlProfilerEventList *model )
{
    if (d->m_eventStatistics)
        disconnect(d->m_eventStatistics,SIGNAL(dataReady()),this,SLOT(buildModel()));
    d->m_eventStatistics = model;
    if (model)
        connect(d->m_eventStatistics,SIGNAL(dataReady()),this,SLOT(buildModel()));
}

void QmlProfilerEventsView::setFieldViewable(Fields field, bool show)
{
    if (field < MaxFields) {
        int length = d->m_fieldShown.count();
        if (field >= length) {
            for (int i=length; i<MaxFields; i++)
                d->m_fieldShown << false;
        }
        d->m_fieldShown[field] = show;
    }
}

void QmlProfilerEventsView::setViewType(ViewTypes type)
{
    d->m_viewType = type;
    switch (type) {
    case EventsView: {
        setObjectName("QmlProfilerEventsView");
        setFieldViewable(Name, true);
        setFieldViewable(Type, true);
        setFieldViewable(Percent, true);
        setFieldViewable(TotalDuration, true);
        setFieldViewable(SelfPercent, false);
        setFieldViewable(SelfDuration, false);
        setFieldViewable(CallCount, true);
        setFieldViewable(TimePerCall, true);
        setFieldViewable(MaxTime, true);
        setFieldViewable(MinTime, true);
        setFieldViewable(MedianTime, true);
        setFieldViewable(Details, true);
        setFieldViewable(Parents, false);
        setFieldViewable(Children, false);
        setShowAnonymousEvents(false);
        setToolTip(QString());
        break;
    }
    case CallersView: {
        setObjectName("QmlProfilerCallersView");
        setFieldViewable(Name, true);
        setFieldViewable(Type, true);
        setFieldViewable(Percent, false);
        setFieldViewable(TotalDuration, false);
        setFieldViewable(SelfPercent, false);
        setFieldViewable(SelfDuration, false);
        setFieldViewable(CallCount, false);
        setFieldViewable(TimePerCall, false);
        setFieldViewable(MaxTime, false);
        setFieldViewable(MinTime, false);
        setFieldViewable(MedianTime, false);
        setFieldViewable(Details, true);
        setFieldViewable(Parents, true);
        setFieldViewable(Children, false);
        setShowAnonymousEvents(true);
        setToolTip(QString());
        break;
    }
    case CalleesView: {
        setObjectName("QmlProfilerCalleesView");
        setFieldViewable(Name, true);
        setFieldViewable(Type, true);
        setFieldViewable(Percent, false);
        setFieldViewable(TotalDuration, false);
        setFieldViewable(SelfPercent, false);
        setFieldViewable(SelfDuration, false);
        setFieldViewable(CallCount, false);
        setFieldViewable(TimePerCall, false);
        setFieldViewable(MaxTime, false);
        setFieldViewable(MinTime, false);
        setFieldViewable(MedianTime, false);
        setFieldViewable(Details, true);
        setFieldViewable(Parents, false);
        setFieldViewable(Children, true);
        setShowAnonymousEvents(true);
        setToolTip(QString());
        break;
    }
    case V8ProfileView: {
        setObjectName("QmlProfilerV8ProfileView");
        setFieldViewable(Name, true);
        setFieldViewable(Type, false);
        setFieldViewable(Percent, true);
        setFieldViewable(TotalDuration, true);
        setFieldViewable(SelfPercent, true);
        setFieldViewable(SelfDuration, true);
        setFieldViewable(CallCount, false);
        setFieldViewable(TimePerCall, false);
        setFieldViewable(MaxTime, false);
        setFieldViewable(MinTime, false);
        setFieldViewable(MedianTime, false);
        setFieldViewable(Details, true);
        setFieldViewable(Parents, false);
        setFieldViewable(Children, false);
        setShowAnonymousEvents(true);
        setToolTip(tr("Trace information from the v8 JavaScript engine. Available only in Qt5 based applications"));
        break;
    }
    default: break;
    }

    buildModel();
}

void QmlProfilerEventsView::setShowAnonymousEvents( bool showThem )
{
    d->m_showAnonymous = showThem;
}

void QmlProfilerEventsView::setHeaderLabels()
{
    int fieldIndex = 0;
    d->m_firstNumericColumn = 0;

    if (d->m_fieldShown[Name]) {
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Location")));
        d->m_firstNumericColumn++;
    }
    if (d->m_fieldShown[Type]) {
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Type")));
        d->m_firstNumericColumn++;
    }
    if (d->m_fieldShown[Percent])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Time in Percent")));
    if (d->m_fieldShown[TotalDuration])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Total Time")));
    if (d->m_fieldShown[SelfPercent])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Self Time in Percent")));
    if (d->m_fieldShown[SelfDuration])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Self Time")));
    if (d->m_fieldShown[CallCount])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Calls")));
    if (d->m_fieldShown[TimePerCall])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Mean Time")));
    if (d->m_fieldShown[MedianTime])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Median Time")));
    if (d->m_fieldShown[MaxTime])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Longest Time")));
    if (d->m_fieldShown[MinTime])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Shortest Time")));
    if (d->m_fieldShown[Details])
        d->m_model->setHeaderData(fieldIndex++, Qt::Horizontal, QVariant(tr("Details")));
}

void QmlProfilerEventsView::clear()
{
    d->m_model->clear();
    d->m_model->setColumnCount(d->getFieldCount());

    setHeaderLabels();
    setSortingEnabled(false);
}

int QmlProfilerEventsView::QmlProfilerEventsViewPrivate::getFieldCount()
{
    int count = 0;
    for (int i=0; i < m_fieldShown.count(); ++i)
        if (m_fieldShown[i] && i != Parents && i != Children)
            count++;
    return count;
}

void QmlProfilerEventsView::buildModel()
{
    if (d->m_eventStatistics) {
        clear();
        if (d->m_viewType == V8ProfileView)
            d->buildV8ModelFromList( d->m_eventStatistics->getV8Events() );
        else
            d->buildModelFromList( d->m_eventStatistics->getEventDescriptions(), d->m_model->invisibleRootItem() );

        bool hasBranches = d->m_fieldShown[Parents] || d->m_fieldShown[Children];
        setRootIsDecorated(hasBranches);

        setSortingEnabled(true);

        if (!hasBranches)
            sortByColumn(d->m_firstNumericColumn,Qt::DescendingOrder);

        expandAll();
        if (d->m_fieldShown[Name])
            resizeColumnToContents(0);

        if (d->m_fieldShown[Type])
            resizeColumnToContents(d->m_fieldShown[Name]?1:0);
        collapseAll();
    }
}

void QmlProfilerEventsView::QmlProfilerEventsViewPrivate::buildModelFromList( const QmlEventDescriptions &list, QStandardItem *parentItem, const QmlEventDescriptions &visitedFunctionsList )
{
    foreach (QmlEventData *binding, list) {
        if (visitedFunctionsList.contains(binding))
            continue;

        if ((!m_showAnonymous) && binding->filename.isEmpty())
            continue;

        QList<QStandardItem *> newRow;
        if (m_fieldShown[Name]) {
            newRow << new EventsViewItem(binding->displayname);
        }

        if (m_fieldShown[Type]) {
            newRow << new EventsViewItem(nameForType(binding->eventType));
            newRow.last()->setData(QVariant(nameForType(binding->eventType)));
        }

        if (m_fieldShown[Percent]) {
            newRow << new EventsViewItem(QString::number(binding->percentOfTime,'f',2)+QLatin1String(" %"));
            newRow.last()->setData(QVariant(binding->percentOfTime));
        }

        if (m_fieldShown[TotalDuration]) {
            newRow << new EventsViewItem(displayTime(binding->cumulatedDuration));
            newRow.last()->setData(QVariant(binding->cumulatedDuration));
        }

        if (m_fieldShown[CallCount]) {
            newRow << new EventsViewItem(QString::number(binding->calls));
            newRow.last()->setData(QVariant(binding->calls));
        }

        if (m_fieldShown[TimePerCall]) {
            newRow << new EventsViewItem(displayTime(binding->timePerCall));
            newRow.last()->setData(QVariant(binding->timePerCall));
        }

        if (m_fieldShown[MedianTime]) {
            newRow << new EventsViewItem(displayTime(binding->medianTime));
            newRow.last()->setData(QVariant(binding->medianTime));
        }

        if (m_fieldShown[MaxTime]) {
            newRow << new EventsViewItem(displayTime(binding->maxTime));
            newRow.last()->setData(QVariant(binding->maxTime));
        }

        if (m_fieldShown[MinTime]) {
            newRow << new EventsViewItem(displayTime(binding->minTime));
            newRow.last()->setData(QVariant(binding->minTime));
        }

        if (m_fieldShown[Details]) {
            newRow << new EventsViewItem(binding->details);
            newRow.last()->setData(QVariant(binding->details));
        }



        if (!newRow.isEmpty()) {
            // no edit
            foreach (QStandardItem *item, newRow)
                item->setEditable(false);

            // metadata
            newRow.at(0)->setData(QVariant(binding->location),LocationRole);
            newRow.at(0)->setData(QVariant(binding->filename),FilenameRole);
            newRow.at(0)->setData(QVariant(binding->line),LineRole);

            // append
            parentItem->appendRow(newRow);

            if (m_fieldShown[Parents] && !binding->parentList.isEmpty()) {
                QmlEventDescriptions newParentList(visitedFunctionsList);
                newParentList.append(binding);

                buildModelFromList(binding->parentList, newRow.at(0), newParentList);
            }

            if (m_fieldShown[Children] && !binding->childrenList.isEmpty()) {
                QmlEventDescriptions newChildrenList(visitedFunctionsList);
                newChildrenList.append(binding);

                buildModelFromList(binding->childrenList, newRow.at(0), newChildrenList);
            }
        }
    }
}

void QmlProfilerEventsView::QmlProfilerEventsViewPrivate::buildV8ModelFromList(const QV8EventDescriptions &list)
{
    foreach (QV8EventData *v8event, list) {

        QList<QStandardItem *> newRow;

        if (m_fieldShown[Name]) {
            newRow << new EventsViewItem(v8event->displayName);
        }

        if (m_fieldShown[Percent]) {
            newRow << new EventsViewItem(QString::number(v8event->totalPercent,'f',2)+QLatin1String(" %"));
            newRow.last()->setData(QVariant(v8event->totalPercent));
        }

        if (m_fieldShown[TotalDuration]) {
            newRow << new EventsViewItem(displayTime(v8event->totalTime));
            newRow.last()->setData(QVariant(v8event->totalTime));
        }

        if (m_fieldShown[SelfPercent]) {
            newRow << new EventsViewItem(QString::number(v8event->selfPercent,'f',2)+QLatin1String(" %"));
            newRow.last()->setData(QVariant(v8event->selfPercent));
        }

        if (m_fieldShown[SelfDuration]) {
            newRow << new EventsViewItem(displayTime(v8event->selfTime));
            newRow.last()->setData(QVariant(v8event->selfTime));
        }

        if (m_fieldShown[Details]) {
            newRow << new EventsViewItem(v8event->functionName);
            newRow.last()->setData(QVariant(v8event->functionName));
        }

        if (!newRow.isEmpty()) {
            // no edit
            foreach (QStandardItem *item, newRow)
                item->setEditable(false);

            // metadata
            newRow.at(0)->setData(QVariant(v8event->filename),FilenameRole);
            newRow.at(0)->setData(QVariant(v8event->line),LineRole);

            // append
            m_model->invisibleRootItem()->appendRow(newRow);
        }
    }
}

QString QmlProfilerEventsView::QmlProfilerEventsViewPrivate::displayTime(double time) const
{
    if (time < 1e6)
        return QString::number(time/1e3,'f',3) + QString::fromWCharArray(L" \u03BCs");
    if (time < 1e9)
        return QString::number(time/1e6,'f',3) + QLatin1String(" ms");

    return QString::number(time/1e9,'f',3) + QLatin1String(" s");
}

QString QmlProfilerEventsView::QmlProfilerEventsViewPrivate::nameForType(int typeNumber) const
{
    switch (typeNumber) {
    case 0: return QmlProfilerEventsView::tr("Paint");
    case 1: return QmlProfilerEventsView::tr("Compile");
    case 2: return QmlProfilerEventsView::tr("Create");
    case 3: return QmlProfilerEventsView::tr("Binding");
    case 4: return QmlProfilerEventsView::tr("Signal");
    }
    return QString();
}

void QmlProfilerEventsView::jumpToItem(const QModelIndex &index)
{
    QStandardItem *clickedItem = d->m_model->itemFromIndex(index);
    QStandardItem *infoItem;
    if (clickedItem->parent())
        infoItem = clickedItem->parent()->child(clickedItem->row(), 0);
    else
        infoItem = d->m_model->item(index.row(), 0);

    int line = infoItem->data(LineRole).toInt();
    if (line == -1)
        return;
    QString fileName = infoItem->data(FilenameRole).toString();
    emit gotoSourceLocation(fileName, line);
}

void QmlProfilerEventsView::contextMenuEvent(QContextMenuEvent *ev)
{
    emit contextMenuRequested(ev->globalPos());
}

QModelIndex QmlProfilerEventsView::selectedItem() const
{
    QModelIndexList sel = selectedIndexes();
    if (sel.isEmpty())
        return QModelIndex();
    else
        return sel.first();
}

QString QmlProfilerEventsView::QmlProfilerEventsViewPrivate::textForItem(QStandardItem *item, bool recursive = true) const
{
    QString str;

    if (recursive) {
        // indentation
        QStandardItem *itemParent = item->parent();
        while (itemParent) {
            str += '\t';
            itemParent = itemParent->parent();
        }
    }

    // item's data
    int colCount = m_model->columnCount();
    for (int j = 0; j < colCount; ++j) {
        QStandardItem *colItem = item->parent() ? item->parent()->child(item->row(),j) : m_model->item(item->row(),j);
        str += colItem->data(Qt::DisplayRole).toString();
        if (j < colCount-1) str += '\t';
    }
    str += '\n';

    // recursively print children
    if (recursive && item->child(0))
        for (int j = 0; j != item->rowCount(); j++)
            str += textForItem(item->child(j));

    return str;
}

void QmlProfilerEventsView::copyTableToClipboard()
{
    QString str;
    int n = d->m_model->rowCount();
    for (int i = 0; i != n; ++i) {
        str += d->textForItem(d->m_model->item(i));
    }
    QClipboard *clipboard = QApplication::clipboard();
#    ifdef Q_WS_X11
    clipboard->setText(str, QClipboard::Selection);
#    endif
    clipboard->setText(str, QClipboard::Clipboard);
}

void QmlProfilerEventsView::copyRowToClipboard()
{
    QString str;
    str = d->textForItem(d->m_model->itemFromIndex(selectedItem()), false);

    QClipboard *clipboard = QApplication::clipboard();
#    ifdef Q_WS_X11
    clipboard->setText(str, QClipboard::Selection);
#    endif
    clipboard->setText(str, QClipboard::Clipboard);
}

} // namespace Internal
} // namespace QmlProfiler
