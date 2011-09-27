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

#ifndef PYTHONPROJECTITEM_H
#define PYTHONPROJECTITEM_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtDeclarative/qdeclarative.h>

namespace PythonProjectManager {

class PythonProjectContentItem : public QObject {
    // base class for all elements that should be direct children of Project element
    Q_OBJECT

public:
    PythonProjectContentItem(QObject *parent = 0) : QObject(parent) {}
};

class PythonProjectItemPrivate;

class PythonProjectItem : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PythonProjectItem)

    Q_PROPERTY(QDeclarativeListProperty<PythonProjectManager::PythonProjectContentItem> content READ content DESIGNABLE false)
    Q_PROPERTY(QString sourceDirectory READ sourceDirectory NOTIFY sourceDirectoryChanged)
    Q_PROPERTY(QStringList importPaths READ importPaths WRITE setImportPaths NOTIFY importPathsChanged)
    Q_PROPERTY(QString mainFile READ mainFile WRITE setMainFile NOTIFY mainFileChanged)

    Q_CLASSINFO("DefaultProperty", "content")

public:
    PythonProjectItem(QObject *parent = 0);
    ~PythonProjectItem();

    QDeclarativeListProperty<PythonProjectContentItem> content();

    QString sourceDirectory() const;
    void setSourceDirectory(const QString &directoryPath);

    QStringList importPaths() const;
    void setImportPaths(const QStringList &paths);

    QStringList files() const;
    bool matchesFile(const QString &filePath) const;

    QString mainFile() const;
    void setMainFile(const QString &mainFilePath);


signals:
    void pythonFilesChanged(const QSet<QString> &, const QSet<QString> &);
    void sourceDirectoryChanged();
    void importPathsChanged();
    void mainFileChanged();

protected:
    PythonProjectItemPrivate *d_ptr;
};

} // namespace PythonProjectManager

//PYTHON_DECLARE_TYPE(PythonProjectManager::PythonProjectItem)
//PYTHON_DECLARE_TYPE(PythonProjectManager::PythonProjectContentItem)
Q_DECLARE_METATYPE(QList<PythonProjectManager::PythonProjectContentItem *>)

#endif // PYTHONPROJECTITEM_H
