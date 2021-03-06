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

#ifndef GCCTOOLCHAIN_H
#define GCCTOOLCHAIN_H

#include "projectexplorer_export.h"

#include "toolchain.h"
#include "abi.h"

namespace ProjectExplorer {

namespace Internal {
class ClangToolChainFactory;
class GccToolChainFactory;
class MingwToolChainFactory;
class LinuxIccToolChainFactory;
}

// --------------------------------------------------------------------------
// GccToolChain
// --------------------------------------------------------------------------

class PROJECTEXPLORER_EXPORT GccToolChain : public ToolChain
{
public:
    QString typeName() const;
    Abi targetAbi() const;
    QString version() const;
    QList<Abi> supportedAbis() const;
    void setTargetAbi(const Abi &);

    bool isValid() const;

    QByteArray predefinedMacros() const;
    QList<HeaderPath> systemHeaderPaths() const;
    void addToEnvironment(Utils::Environment &env) const;
    QString mkspec() const;
    QString makeCommand() const;
    void setDebuggerCommand(const QString &);
    QString debuggerCommand() const;
    IOutputParser *outputParser() const;

    QVariantMap toMap() const;
    bool fromMap(const QVariantMap &data);

    ToolChainConfigWidget *configurationWidget();

    bool operator ==(const ToolChain &) const;

    virtual void setCompilerPath(const QString &);
    QString compilerPath() const;

    ToolChain *clone() const;

protected:
    GccToolChain(const QString &id, bool autodetect);
    GccToolChain(const GccToolChain &);

    QString defaultDisplayName() const;

    void updateId();

    virtual QList<Abi> detectSupportedAbis() const;
    virtual QString detectVersion() const;

    mutable QByteArray m_predefinedMacros;

private:
    GccToolChain(bool autodetect);

    void updateSupportedAbis() const;

    QString m_compilerPath;
    QString m_debuggerCommand;

    Abi m_targetAbi;
    mutable QList<Abi> m_supportedAbis;
    mutable QList<HeaderPath> m_headerPathes;
    mutable QString m_version;

    friend class Internal::GccToolChainFactory;
    friend class ToolChainFactory;
};

// --------------------------------------------------------------------------
// ClangToolChain
// --------------------------------------------------------------------------

class PROJECTEXPLORER_EXPORT ClangToolChain : public GccToolChain
{
public:
    QString typeName() const;
    QString makeCommand() const;
    QString mkspec() const;

    IOutputParser *outputParser() const;

    ToolChain *clone() const;

private:
    ClangToolChain(bool autodetect);

    friend class Internal::ClangToolChainFactory;
    friend class ToolChainFactory;
};

// --------------------------------------------------------------------------
// MingwToolChain
// --------------------------------------------------------------------------

class PROJECTEXPLORER_EXPORT MingwToolChain : public GccToolChain
{
public:
    QString typeName() const;
    QString mkspec() const;
    QString makeCommand() const;

    ToolChain *clone() const;

private:
    MingwToolChain(bool autodetect);

    friend class Internal::MingwToolChainFactory;
    friend class ToolChainFactory;
};

// --------------------------------------------------------------------------
// LinuxIccToolChain
// --------------------------------------------------------------------------

class PROJECTEXPLORER_EXPORT LinuxIccToolChain : public GccToolChain
{
public:

    QString typeName() const;

    IOutputParser *outputParser() const;

    QString mkspec() const;

    ToolChain *clone() const;

private:
    LinuxIccToolChain(bool autodetect);

    friend class Internal::LinuxIccToolChainFactory;
    friend class ToolChainFactory;
};

} // namespace ProjectExplorer

#endif // GCCTOOLCHAIN_H
