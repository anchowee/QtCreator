/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
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

#ifndef VALGRIND_PROTOCOL_MEMCHECKRUNNER_H
#define VALGRIND_PROTOCOL_MEMCHECKRUNNER_H

#include <valgrindrunner.h>

namespace Valgrind {

namespace XmlProtocol{
class ThreadedParser;
}

namespace Memcheck {

class MemcheckRunner : public ValgrindRunner
{
    Q_OBJECT

public:
    explicit MemcheckRunner(QObject *parent = 0);
    ~MemcheckRunner();

    void setParser(XmlProtocol::ThreadedParser *parser);

    void start();
    void startRemotely(const Utils::SshConnectionParameters &sshParams);

signals:
    void logMessageReceived(const QByteArray &);

private slots:
    void xmlSocketConnected();
    void logSocketConnected();
    void readLogSocket();

private:
    QString tool() const;

    class Private;
    Private *d;
};

} // namespace Memcheck
} // namespace Valgrind

#endif // VALGRIND_PROTOCOL_MEMCHECKRUNNER_H
