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
#ifndef MADDEUPLOADANDINSTALLPACKAGESTEPS_H
#define MADDEUPLOADANDINSTALLPACKAGESTEPS_H

#include <remotelinux/abstractremotelinuxdeploystep.h>

namespace RemoteLinux {
class AbstractUploadAndInstallPackageService;
}

namespace Madde {
namespace Internal {

class MaemoUploadAndInstallPackageStep : public RemoteLinux::AbstractRemoteLinuxDeployStep
{
    Q_OBJECT
public:
    MaemoUploadAndInstallPackageStep(ProjectExplorer::BuildStepList *bsl);
    MaemoUploadAndInstallPackageStep(ProjectExplorer::BuildStepList *bsl,
        MaemoUploadAndInstallPackageStep *other);

    bool isDeploymentPossible(QString *whyNot = 0) const;

    static QString stepId();
    static QString displayName();

private:
    RemoteLinux::AbstractRemoteLinuxDeployService *deployService() const;

    void ctor();

    RemoteLinux::AbstractUploadAndInstallPackageService *m_deployService;
};


class MeegoUploadAndInstallPackageStep : public RemoteLinux::AbstractRemoteLinuxDeployStep
{
    Q_OBJECT
public:
    MeegoUploadAndInstallPackageStep(ProjectExplorer::BuildStepList *bsl);
    MeegoUploadAndInstallPackageStep(ProjectExplorer::BuildStepList *bsl,
        MeegoUploadAndInstallPackageStep *other);

    bool isDeploymentPossible(QString *whyNot) const;

    static QString stepId();
    static QString displayName();

private:
    RemoteLinux::AbstractRemoteLinuxDeployService *deployService() const;

    void ctor();

    RemoteLinux::AbstractUploadAndInstallPackageService *m_deployService;
};

} // namespace Internal
} // namespace Madde

#endif // MADDEUPLOADANDINSTALLPACKAGESTEPS_H
