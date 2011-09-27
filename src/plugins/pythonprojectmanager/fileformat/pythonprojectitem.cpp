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

#include "pythonprojectitem.h"
#include "filefilteritems.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>

namespace PythonProjectManager {

class PythonProjectItemPrivate : public QObject {
    Q_OBJECT

public:
    QString sourceDirectory;
    QStringList importPaths;
    QStringList absoluteImportPaths;
    QString mainFile;

    QList<PythonFileFilterItem*> pythonFileFilters() const;

    // content property
    QList<PythonProjectContentItem*> content;
};

QList<PythonFileFilterItem*> PythonProjectItemPrivate::pythonFileFilters() const
{
    QList<PythonFileFilterItem*> pythonFilters;
    for (int i = 0; i < content.size(); ++i) {
        PythonProjectContentItem *contentElement = content.at(i);
        PythonFileFilterItem *pythonFileFilter = qobject_cast<PythonFileFilterItem*>(contentElement);
        if (pythonFileFilter) {
            pythonFilters << pythonFileFilter;
        }
    }
    return pythonFilters;
}

PythonProjectItem::PythonProjectItem(QObject *parent) :
        QObject(parent),
        d_ptr(new PythonProjectItemPrivate)
{
//    Q_D(PythonProjectItem);
//
//    PythonFileFilter *defaultPythonFilter = new PythonFileFilter(this);
//    d->content.append(defaultPythonFilter);
}

PythonProjectItem::~PythonProjectItem()
{
    delete d_ptr;
}

QDeclarativeListProperty<PythonProjectContentItem> PythonProjectItem::content()
{
    Q_D(PythonProjectItem);
    return QDeclarativeListProperty<PythonProjectContentItem>(this, d->content);
}

QString PythonProjectItem::sourceDirectory() const
{
    Q_D(const PythonProjectItem);
    return d->sourceDirectory;
}

// kind of initialization
void PythonProjectItem::setSourceDirectory(const QString &directoryPath)
{
    Q_D(PythonProjectItem);

    if (d->sourceDirectory == directoryPath)
        return;

    d->sourceDirectory = directoryPath;

    for (int i = 0; i < d->content.size(); ++i) {
        PythonProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            fileFilter->setDefaultDirectory(directoryPath);
            connect(fileFilter, SIGNAL(filesChanged(QSet<QString>, QSet<QString>)),
                    this, SIGNAL(pythonFilesChanged(QSet<QString>, QSet<QString>)));
        }
    }

    setImportPaths(d->importPaths);

    emit sourceDirectoryChanged();
}

QStringList PythonProjectItem::importPaths() const
{
    Q_D(const PythonProjectItem);
    return d->absoluteImportPaths;
}

void PythonProjectItem::setImportPaths(const QStringList &importPaths)
{
    Q_D(PythonProjectItem);

    if (d->importPaths != importPaths)
        d->importPaths = importPaths;

    // convert to absolute paths
    QStringList absoluteImportPaths;
    const QDir sourceDir(sourceDirectory());
    foreach (const QString &importPath, importPaths)
        absoluteImportPaths += QDir::cleanPath(sourceDir.absoluteFilePath(importPath));

    if (d->absoluteImportPaths == absoluteImportPaths)
        return;

    d->absoluteImportPaths = absoluteImportPaths;
    emit importPathsChanged();
}

/* Returns list of absolute paths */
QStringList PythonProjectItem::files() const
{
    Q_D(const PythonProjectItem);
    QStringList files;

    for (int i = 0; i < d->content.size(); ++i) {
        PythonProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            foreach (const QString &file, fileFilter->files()) {
                if (!files.contains(file))
                    files << file;
            }
        }
    }
    return files;
}

/**
  Check whether the project would include a file path
  - regardless whether the file already exists or not.

  @param filePath: absolute file path to check
  */
bool PythonProjectItem::matchesFile(const QString &filePath) const
{
    Q_D(const PythonProjectItem);
    for (int i = 0; i < d->content.size(); ++i) {
        PythonProjectContentItem *contentElement = d->content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            if (fileFilter->matchesFile(filePath))
                return true;
        }
    }
    return false;
}

QString PythonProjectItem::mainFile() const
{
    Q_D(const PythonProjectItem);
    return d->mainFile;
}

void PythonProjectItem::setMainFile(const QString &mainFilePath)
{
    Q_D(PythonProjectItem);
    if (mainFilePath == d->mainFile)
        return;
    d->mainFile = mainFilePath;
    emit mainFileChanged();
}

} // namespace PythonProjectManager

#include "pythonprojectitem.moc"
