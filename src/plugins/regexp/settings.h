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

#ifndef REGEXP_SETTINGS_H
#define REGEXP_SETTINGS_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace RegExp {
namespace Internal {

// Settings of the Regexp plugin.
// Provides methods to serialize into QSettings.

struct Settings {
    Settings();
    void clear();
    void fromQSettings(QSettings *s);
    void toQSettings(QSettings *s) const;

    QRegExp::PatternSyntax m_patternSyntax;
    bool m_minimal;
    bool m_caseSensitive;

    QStringList m_patterns;
    QString m_currentPattern;
    QStringList m_matches;
    QString m_currentMatch;
};

} // namespace Internal
} // namespace RegExp

#endif // REGEXP_SETTINGS_H
