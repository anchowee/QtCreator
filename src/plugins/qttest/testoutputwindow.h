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

#ifndef TESTOUTPUTWINDOW_H
#define TESTOUTPUTWINDOW_H

#include <coreplugin/ioutputpane.h>

QT_FORWARD_DECLARE_CLASS(QTextEdit)

QTextEdit *testOutputPane();

class TestOutputWindow : public Core::IOutputPane
{
    Q_OBJECT

public:
    TestOutputWindow();
    ~TestOutputWindow();

    virtual QString displayName() const { return "Test Output"; }

    QWidget *outputWidget(QWidget *parent);
    QList<QWidget *> toolBarWidgets() const { return QList<QWidget *>(); }

    QString name() const;
    int priorityInStatusBar() const;
    void clearContents();
    void visibilityChanged(bool visible);

    bool canFocus() const;
    bool hasFocus() const;
    void setFocus();

    virtual bool canNext() const;
    virtual bool canPrevious() const;
    virtual void goToNext();
    virtual void goToPrev();
    bool canNavigate() const;

    QTextEdit *m_widget;
};

#endif // TESTOUTPUTWINDOW_H
