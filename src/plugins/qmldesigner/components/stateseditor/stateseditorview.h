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

#ifndef STATESEDITORVIEW_H
#define STATESEDITORVIEW_H

#include <qmlmodelview.h>

namespace QmlDesigner {


class StatesEditorModel;
class StatesEditorWidget;

class StatesEditorView : public QmlModelView {
    Q_OBJECT

public:
    explicit StatesEditorView(QObject *parent = 0);

    void renameState(int nodeId,const QString &newName);
    bool validStateName(const QString &name) const;
    QString currentStateName() const;

    void nodeInstancePropertyChanged(const ModelNode &node, const QString &propertyName);

    // AbstractView
    void modelAttached(Model *model);
    void modelAboutToBeDetached(Model *model);
    void propertiesRemoved(const QList<AbstractProperty>& propertyList);
    void variantPropertiesChanged(const QList<VariantProperty>& propertyList, PropertyChangeFlags propertyChange);

    void nodeAboutToBeRemoved(const ModelNode &removedNode);
    void nodeRemoved(const ModelNode &removedNode, const NodeAbstractProperty &parentProperty, PropertyChangeFlags propertyChange);
    void nodeAboutToBeReparented(const ModelNode &node, const NodeAbstractProperty &newPropertyParent, const NodeAbstractProperty &oldPropertyParent, AbstractView::PropertyChangeFlags propertyChange);
    void nodeReparented(const ModelNode &node, const NodeAbstractProperty &newPropertyParent, const NodeAbstractProperty &oldPropertyParent, AbstractView::PropertyChangeFlags propertyChange);
    void nodeOrderChanged(const NodeListProperty &listProperty, const ModelNode &movedNode, int oldIndex);


    // QmlModelView
    void actualStateChanged(const ModelNode &node);
    void transformChanged(const QmlObjectNode &qmlObjectNode, const QString &propertyName);
    void parentChanged(const QmlObjectNode &qmlObjectNode);
    void otherPropertyChanged(const QmlObjectNode &qmlObjectNode, const QString &propertyName);

    void scriptFunctionsChanged(const ModelNode &node, const QStringList &scriptFunctionList);
    void nodeIdChanged(const ModelNode &node, const QString &newId, const QString &oldId);
    void bindingPropertiesChanged(const QList<BindingProperty> &propertyList, PropertyChangeFlags propertyChange);
    void selectedNodesChanged(const QList<ModelNode> &selectedNodeList, const QList<ModelNode> &lastSelectedNodeList);

    void instancesPreviewImageChanged(const QVector<ModelNode> &nodeList);

    StatesEditorWidget *widget();

public slots:
    void synchonizeCurrentStateFromWidget();
    void createNewState();
    void removeState(int nodeId);

private:
    void resetModel();
    void addState();
    void duplicateCurrentState();

private:
    QWeakPointer<StatesEditorModel> m_statesEditorModel;
    QWeakPointer<StatesEditorWidget> m_statesEditorWidget;
    int m_lastIndex;
};

} // namespace QmlDesigner

#endif // STATESEDITORVIEW_H
