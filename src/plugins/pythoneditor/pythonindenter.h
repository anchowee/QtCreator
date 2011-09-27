/*
 * Copyright (c) 2011 Instituto Nokia de Tecnologia
 *   Author: Paulo Alcantara <paulo.alcantara@openbossa.org>
 *
 * This software may be redistributed and/or modified under the terms of
 * the GNU General Public License ("GPL") version 2 as published by the
 * Free Software Foundation.
 */

#ifndef _PYTHONINDENTER_H_
#define _PYTHONINDENTER_H_

#include <texteditor/indenter.h>
#include <texteditor/typingsettings.h>

namespace PythonEditor {
namespace Internal {

class Context;

class Q_DECL_EXPORT Indenter : public TextEditor::Indenter
{
public:
    Indenter();
    virtual ~Indenter();

    virtual bool isElectricCharacter(const QChar &ch) const;
    virtual void indentBlock(QTextDocument *doc,
                             const QTextBlock &block,
                             const QChar &typedChar,
                             const TextEditor::TabSettings &tabSettings);
    virtual void setCodeStylePreferences(TextEditor::ICodeStylePreferences *cs);
    virtual void invalidateCache(QTextDocument *doc);
    virtual void setTypingSettings(const TextEditor::TypingSettings &ts);

    unsigned m_tabSize;
};

}
}

#endif /* _PYTHONINDENTER_H_ */
