#include "pythonprojectapplicationwizard.h"

#include "pythonprojectconstants.h"

#include <app/app_version.h>
#include <projectexplorer/customwizard/customwizard.h>
#include <qtsupport/qtsupportconstants.h>

#include <QtGui/QIcon>

#include <QtGui/QPainter>
#include <QtGui/QPixmap>

#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

namespace PythonProjectManager {
namespace Internal {

PythonProjectApplicationWizardDialog::PythonProjectApplicationWizardDialog(QWidget *parent) :
    ProjectExplorer::BaseProjectWizardDialog(parent)
{
    setWindowTitle(tr("New Python project"));
    setIntroDescription(tr("This wizard generates a Python project."));
}

PythonProjectApplicationWizard::PythonProjectApplicationWizard()
    : Core::BaseFileWizard(parameters())
{ }

PythonProjectApplicationWizard::~PythonProjectApplicationWizard()
{ }

Core::BaseFileWizardParameters PythonProjectApplicationWizard::parameters()
{
    Core::BaseFileWizardParameters parameters(ProjectWizard);
    parameters.setIcon(QIcon(QLatin1String(QtSupport::Constants::QML_WIZARD_ICON)));
    parameters.setDisplayName(tr("Python Project"));
    parameters.setId(QLatin1String("QB.PYTHON Application"));

    parameters.setDescription(tr("Creates a Python project with a stub "
                                 "implementation"));
    parameters.setCategory(QLatin1String(QtSupport::Constants::QML_WIZARD_CATEGORY));
    parameters.setDisplayCategory(QCoreApplication::translate(QtSupport::Constants::QML_WIZARD_TR_SCOPE,
                                                              QtSupport::Constants::QML_WIZARD_TR_CATEGORY));
    return parameters;
}

QWizard *PythonProjectApplicationWizard::createWizardDialog(QWidget *parent,
                                                  const QString &defaultPath,
                                                  const WizardPageList &extensionPages) const
{
    PythonProjectApplicationWizardDialog *wizard = new PythonProjectApplicationWizardDialog(parent);

    wizard->setPath(defaultPath);
    wizard->setProjectName(PythonProjectApplicationWizardDialog::uniqueProjectName(defaultPath));

    foreach (QWizardPage *p, extensionPages)
        BaseFileWizard::applyExtensionPageShortTitle(wizard, wizard->addPage(p));

    return wizard;
}

Core::GeneratedFiles PythonProjectApplicationWizard::generateFiles(const QWizard *w,
                                                     QString *errorMessage) const
{
    Q_UNUSED(errorMessage)

    const PythonProjectApplicationWizardDialog *wizard = qobject_cast<const PythonProjectApplicationWizardDialog *>(w);
    const QString projectName = wizard->projectName();
    const QString projectPath = wizard->path() + QLatin1Char('/') + projectName;

    const QString mainFileName =
            Core::BaseFileWizard::buildFileName(projectPath, projectName,
                                                QLatin1String("py"));

    QString contents;
    {
        QTextStream out(&contents);

        out
            << "print \"Hello, world.\"" << endl;
    }
    Core::GeneratedFile generatedMainFile(mainFileName);
    generatedMainFile.setContents(contents);
    generatedMainFile.setAttributes(Core::GeneratedFile::OpenEditorAttribute);

    Core::GeneratedFiles files;
    files.append(generatedMainFile);

    return files;
}

bool PythonProjectApplicationWizard::postGenerateFiles(const QWizard *, const Core::GeneratedFiles &l, QString *errorMessage)
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);

}

} // namespace Internal
} // namespace PythonProjectManager

