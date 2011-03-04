/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Nicolas Arnaud-Cormos, KDAB (nicolas.arnaud-cormos@kdab.com)
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "analyzerconstants.h"

#include "analyzeroptionspage.h"
#include "analyzersettings.h"

#include <QCoreApplication>
#include <QLatin1String>
#include <QWidget>
#include <QDebug>
#include <QIcon>

#include <coreplugin/icore.h>

using namespace Analyzer;
using namespace Analyzer::Internal;

AnalyzerOptionsPage::AnalyzerOptionsPage(AbstractAnalyzerSubConfig *config, QObject *parent) :
    Core::IOptionsPage(parent),
    m_config(config)
{
}

QString AnalyzerOptionsPage::id() const
{
    return m_config->id();
}

QString AnalyzerOptionsPage::displayName() const
{
    return m_config->displayName();
}

QString AnalyzerOptionsPage::category() const
{
    return QLatin1String(Constants::ANALYZER_SETTINGS_CATEGORY);
}

QString AnalyzerOptionsPage::displayCategory() const
{
    return QCoreApplication::tr("Analyzer", Constants::ANALYZER_SETTINGS_TR_CATEGORY);
}

QIcon AnalyzerOptionsPage::categoryIcon() const
{
    return QIcon(QLatin1String(Constants::ANALYZER_SETTINGS_CATEGORY_ICON));
}

QWidget *AnalyzerOptionsPage::createPage(QWidget *parent)
{
    return m_config->createConfigWidget(parent);
}

void AnalyzerOptionsPage::apply()
{
    AnalyzerGlobalSettings::instance()->writeSettings();
}

void AnalyzerOptionsPage::finish()
{
}