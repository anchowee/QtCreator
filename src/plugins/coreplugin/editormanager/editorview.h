/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.3, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QSplitter>
#include <QtGui/QStackedLayout>

#include <coreplugin/icontext.h>

#include <QtCore/QMap>
#include <QtGui/QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
class QComboBox;
class QToolBar;
class QToolButton;
class QLabel;
class QStackedWidget;
QT_END_NAMESPACE

namespace Core {

class IEditor;

namespace Internal {

class EditorModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    EditorModel(QObject *parent) : QAbstractItemModel(parent) {}
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex &/*index*/) const { return QModelIndex(); }
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

    void addEditor(IEditor *editor, bool isDuplicate = false);

    void removeEditor(IEditor *editor);
    void emitDataChanged(IEditor *editor);

    QList<IEditor *> editors() const { return m_editors; }
    bool isDuplicate(IEditor *editor) const;
    QList<IEditor *> duplicatesFor(IEditor *editor) const;
    IEditor *originalForDuplicate(IEditor *duplicate) const;
    QModelIndex indexOf(IEditor *editor) const;
    QModelIndex indexOf(const QString &filename) const;

private slots:
    void itemChanged();
private:
    QList<IEditor *> m_editors;
    QList<IEditor *>m_duplicateEditors;
};



class EditorView : public QWidget
{
    Q_OBJECT

public:
    EditorView(EditorModel *model = 0, QWidget *parent = 0);
    virtual ~EditorView();

    int editorCount() const;
    void addEditor(IEditor *editor);
    void removeEditor(IEditor *editor);
    IEditor *currentEditor() const;
    void setCurrentEditor(IEditor *editor);

    bool hasEditor(IEditor *editor) const;

    QList<IEditor *> editors() const;
    void showEditorInfoBar(const QString &kind,
                           const QString &infoText,
                           const QString &buttonText,
                           QObject *object, const char *member);
    void hideEditorInfoBar(const QString &kind);


public slots:
    void closeView();

private slots:
    void updateEditorStatus(Core::IEditor *editor = 0);
    void checkEditorStatus();
    void makeEditorWritable();
    void listSelectionActivated(int index);

private:
    void updateToolBar(IEditor *editor);
    void checkProjectLoaded(IEditor *editor);

    QWidget *m_toolBar;
    QToolBar *m_activeToolBar;
    QStackedWidget *m_container;
    QComboBox *m_editorList;
    QToolButton *m_closeButton;
    QToolButton *m_lockButton;
    QToolBar *m_defaultToolBar;
    QString m_infoWidgetKind;
    QFrame *m_infoWidget;
    QLabel *m_infoWidgetLabel;
    QToolButton *m_infoWidgetButton;
    IEditor *m_editorForInfoWidget;
    QSortFilterProxyModel m_proxyModel;
    QList<IEditor *>m_editors;
    QMap<QWidget *, IEditor *> m_widgetEditorMap;
};

class SplitterOrView  : public QWidget
{
    Q_OBJECT
public:
    SplitterOrView(Internal::EditorModel *model = 0); // creates a splitter with an empty view
    SplitterOrView(Core::IEditor *editor);

    void split(Qt::Orientation orientation);
    void unsplit();

    inline bool isView() const { return m_view != 0; }
    inline void setRoot(bool b) { m_isRoot = b; }
    inline bool isRoot() const { return m_isRoot; }
    
    inline bool isSplitter() const { return m_splitter != 0; }
    inline Core::IEditor *editor() const { return m_view ? m_view->currentEditor() : 0; }
    inline QList<Core::IEditor *> editors() const { return m_view ? m_view->editors() : QList<Core::IEditor*>(); }
    inline bool hasEditor(Core::IEditor *editor) const { return m_view && m_view->hasEditor(editor); }
    inline bool hasEditors() const { return m_view && m_view->editorCount() != 0; }
    inline EditorView *view() const { return m_view; }
    inline QSplitter *splitter() const { return m_splitter; }


    SplitterOrView *findView(Core::IEditor *editor);
    SplitterOrView *findView(EditorView *view);
    SplitterOrView *findFirstView();
    SplitterOrView *findEmptyView();
    SplitterOrView *findSplitter(Core::IEditor *editor);
    SplitterOrView *findSplitter(SplitterOrView *child);

    SplitterOrView *findNextView(SplitterOrView *view);

    QSize sizeHint() const { return minimumSizeHint(); }
    QSize minimumSizeHint() const;

protected:
    void focusInEvent(QFocusEvent *);
    void paintEvent(QPaintEvent *);


private:
    void close();
    void closeSplitterEditors();
    SplitterOrView *findNextView_helper(SplitterOrView *view, bool *found);
    bool m_isRoot;
    QStackedLayout *m_layout;
    EditorView *m_view;
    QSplitter *m_splitter;
};



}
}
#endif // EDITORVIEW_H
