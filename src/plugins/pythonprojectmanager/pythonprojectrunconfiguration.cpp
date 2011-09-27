#include "pythonprojectrunconfiguration.h"
#include "pythonproject.h"
#include "pythonprojectmanagerconstants.h"
#include "pythonprojecttarget.h"
#include "pythonprojectrunconfigurationwidget.h"
#include <coreplugin/mimedatabase.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/icore.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>
#include <qtsupport/qtversionmanager.h>
#include <qtsupport/qtoutputformatter.h>
#include <qtsupport/qtsupportconstants.h>

#ifdef Q_OS_WIN
#include <utils/winutils.h>
#endif

using Core::EditorManager;
using Core::ICore;
using Core::IEditor;
using QtSupport::QtVersionManager;

using namespace PythonProjectManager::Internal;

namespace PythonProjectManager {

const char * const M_CURRENT_FILE = "CurrentFile";

PythonProjectRunConfiguration::PythonProjectRunConfiguration(PythonProjectTarget *parent) :
    ProjectExplorer::RunConfiguration(parent, QLatin1String(Constants::PYTHON_RC_ID)),
    m_scriptFile(M_CURRENT_FILE),
    m_projectTarget(parent),
    m_usingCurrentFile(true),
    m_isEnabled(false)
{
    qDebug() << __func__;
    ctor();
}

PythonProjectRunConfiguration::PythonProjectRunConfiguration(PythonProjectTarget *parent,
                                                       PythonProjectRunConfiguration *source) :
    ProjectExplorer::RunConfiguration(parent, source),
    m_scriptFile(source->m_scriptFile),
    m_pythonArgs(source->m_pythonArgs),
    m_projectTarget(parent),
    m_usingCurrentFile(source->m_usingCurrentFile),
    m_isEnabled(source->m_isEnabled),
    m_userEnvironmentChanges(source->m_userEnvironmentChanges)
{
    qDebug() << __func__;
    ctor();
}

bool PythonProjectRunConfiguration::isEnabled() const
{
    return m_isEnabled;
}

void PythonProjectRunConfiguration::ctor()
{
    EditorManager *em = Core::EditorManager::instance();
    connect(em, SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(changeCurrentFile(Core::IEditor*)));

    QtVersionManager *qtVersions = QtVersionManager::instance();
    connect(qtVersions, SIGNAL(qtVersionsChanged(QList<int>)), this, SLOT(updateQtVersions()));

    setDisplayName(tr("PYTHON Viewer", "PYTHONRunConfiguration display name."));
}

PythonProjectRunConfiguration::~PythonProjectRunConfiguration()
{
}

PythonProjectTarget *PythonProjectRunConfiguration::pythonTarget() const
{
    return static_cast<PythonProjectTarget *>(target());
}

QString PythonProjectRunConfiguration::pythonPath() const
{
    return QString("/usr/bin/python");
}

QString PythonProjectRunConfiguration::pythonArguments() const
{
    // arguments in .user file
    QString args = m_pythonArgs;

    qDebug() << "args: " << args;

    QString s = mainScript();
    if (!s.isEmpty()) {
        s = canonicalCapsPath(s);
        Utils::QtcProcess::addArg(&args, s);
    }

    return args;
}

QString PythonProjectRunConfiguration::workingDirectory() const
{
    QFileInfo projectFile(pythonTarget()->pythonProject()->file()->fileName());
    return canonicalCapsPath(projectFile.absolutePath());
}

/* QtDeclarative checks explicitly that the capitalization for any URL / path
   is exactly like the capitalization on disk.*/
QString PythonProjectRunConfiguration::canonicalCapsPath(const QString &fileName)
{
    QString canonicalPath = QFileInfo(fileName).canonicalFilePath();

#if defined(Q_OS_WIN)
    canonicalPath = Utils::normalizePathName(canonicalPath);
#endif

    return canonicalPath;
}

QWidget *PythonProjectRunConfiguration::createConfigurationWidget()
{
    QTC_ASSERT(m_configurationWidget.isNull(), return m_configurationWidget.data());
    m_configurationWidget = new PythonProjectRunConfigurationWidget(this);
    return m_configurationWidget.data();
}

Utils::OutputFormatter *PythonProjectRunConfiguration::createOutputFormatter() const
{
    return new QtSupport::QtOutputFormatter(pythonTarget()->pythonProject());
}

PythonProjectRunConfiguration::MainScriptSource PythonProjectRunConfiguration::mainScriptSource() const
{
    if (m_usingCurrentFile) {
        return FileInEditor;
    }
    if (!m_mainScriptFilename.isEmpty()) {
        return FileInSettings;
    }
    return FileInProjectFile;
}

/**
  Returns absolute path to main script file.
  */
QString PythonProjectRunConfiguration::mainScript() const
{
    if (m_usingCurrentFile) {
        return m_currentFileFilename;
    }

    if (!m_mainScriptFilename.isEmpty()) {
        return m_mainScriptFilename;
    }

    const QString path = pythonTarget()->pythonProject()->mainFile();
    if (path.isEmpty()) {
        return m_currentFileFilename;
    }
    if (QFileInfo(path).isAbsolute()) {
        return path;
    } else {
        return pythonTarget()->pythonProject()->projectDir().absoluteFilePath(path);
    }
}

void PythonProjectRunConfiguration::setScriptSource(MainScriptSource source,
                                                 const QString &settingsPath)
{
    if (source == FileInEditor) {
        m_scriptFile = M_CURRENT_FILE;
        m_mainScriptFilename.clear();
        m_usingCurrentFile = true;
    } else if (source == FileInProjectFile) {
        m_scriptFile.clear();
        m_mainScriptFilename.clear();
        m_usingCurrentFile = false;
    } else { // FileInSettings
        m_scriptFile = settingsPath;
        m_mainScriptFilename
                = pythonTarget()->pythonProject()->projectDir().absoluteFilePath(m_scriptFile);
        m_usingCurrentFile = false;
    }
    updateEnabled();
    if (m_configurationWidget)
        m_configurationWidget.data()->updateFileComboBox();
}

Utils::Environment PythonProjectRunConfiguration::environment() const
{
    Utils::Environment env = baseEnvironment();
    env.modify(userEnvironmentChanges());
    return env;
}

ProjectExplorer::Abi PythonProjectRunConfiguration::abi() const
{
    ProjectExplorer::Abi hostAbi = ProjectExplorer::Abi::hostAbi();
    return ProjectExplorer::Abi(hostAbi.architecture(), hostAbi.os(), hostAbi.osFlavor(),
                                ProjectExplorer::Abi::RuntimeQmlFormat, hostAbi.wordWidth());
}

QVariantMap PythonProjectRunConfiguration::toMap() const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());

    map.insert(QLatin1String(Constants::PYTHON_VIEWER_ARGUMENTS_KEY),
                m_pythonArgs);
    map.insert(QLatin1String(Constants::PYTHON_MAINSCRIPT_KEY), m_scriptFile);
    map.insert(QLatin1String(Constants::USER_ENVIRONMENT_CHANGES_KEY),
               Utils::EnvironmentItem::toStringList(m_userEnvironmentChanges));
    return map;
}

bool PythonProjectRunConfiguration::fromMap(const QVariantMap &map)
{
    m_pythonArgs = map.value(QLatin1String(Constants::PYTHON_VIEWER_ARGUMENTS_KEY)).toString();
    m_scriptFile = map.value(QLatin1String(Constants::PYTHON_MAINSCRIPT_KEY), M_CURRENT_FILE).toString();
    m_userEnvironmentChanges = Utils::EnvironmentItem::fromStringList(
                map.value(QLatin1String(Constants::USER_ENVIRONMENT_CHANGES_KEY)).toStringList());


    if (m_scriptFile == M_CURRENT_FILE) {
        setScriptSource(FileInEditor);
    } else if (m_scriptFile.isEmpty()) {
        setScriptSource(FileInProjectFile);
    } else {
        setScriptSource(FileInSettings, m_scriptFile);
    }

    return RunConfiguration::fromMap(map);
}

void PythonProjectRunConfiguration::changeCurrentFile(Core::IEditor *editor)
{
    if (editor) {
        m_currentFileFilename = editor->file()->fileName();
    }
    updateEnabled();
}

void PythonProjectRunConfiguration::updateEnabled()
{
    bool pythonFileFound = false;
    if (m_usingCurrentFile) {
        Core::IEditor *editor = Core::EditorManager::instance()->currentEditor();
        Core::MimeDatabase *db = ICore::instance()->mimeDatabase();
        if (editor) {
            m_currentFileFilename = editor->file()->fileName();
            if (db->findByFile(mainScript()).type() == QLatin1String("application/x-python"))
                pythonFileFound = true;
        }
        if (!editor
                || db->findByFile(mainScript()).type() == QLatin1String("application/x-pythonproject")) {
            // find a python file with lowercase filename. This is slow, but only done
            // in initialization/other border cases.
            foreach(const QString &filename, m_projectTarget->pythonProject()->files()) {
                const QFileInfo fi(filename);

                if (!filename.isEmpty() && fi.baseName()[0].isLower()
                        && db->findByFile(fi).type() == QLatin1String("application/x-python"))
                {
                    m_currentFileFilename = filename;
                    pythonFileFound = true;
                    break;
                }

            }
        }
    } else { // use default one
        pythonFileFound = !mainScript().isEmpty();
    }

    bool newValue = (QFileInfo(pythonPath()).exists() || pythonFileFound);


    // Always emit change signal to force reevaluation of run/debug buttons
    m_isEnabled = newValue;
    emit isEnabledChanged(m_isEnabled);
}

Utils::Environment PythonProjectRunConfiguration::baseEnvironment() const
{
    Utils::Environment env;

    return env;
}

void PythonProjectRunConfiguration::setUserEnvironmentChanges(const QList<Utils::EnvironmentItem> &diff)
{
    if (m_userEnvironmentChanges != diff) {
        m_userEnvironmentChanges = diff;
        if (m_configurationWidget)
            m_configurationWidget.data()->userEnvironmentChangesChanged();
    }
}

QList<Utils::EnvironmentItem> PythonProjectRunConfiguration::userEnvironmentChanges() const
{
    return m_userEnvironmentChanges;
}

} // namespace PythonProjectManager
