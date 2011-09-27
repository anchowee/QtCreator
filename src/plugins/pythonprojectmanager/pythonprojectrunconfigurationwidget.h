#ifndef PYTHONPROJECTRUNCONFIGURATIONWIDGET_H
#define PYTHONPROJECTRUNCONFIGURATIONWIDGET_H

#include <QtGui/QWidget>

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QStandardItemModel)

namespace ProjectExplorer {

class EnvironmentWidget;

} // namespace Qt4ProjectManager

namespace PythonProjectManager {

class PythonProjectRunConfiguration;

namespace Internal {

const char * const CURRENT_FILE  = QT_TRANSLATE_NOOP("PythonManager",
                                                     "<Current File>");

class PythonProjectRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PythonProjectRunConfigurationWidget(PythonProjectRunConfiguration *rc);

public slots:
    void userEnvironmentChangesChanged();
    void updateFileComboBox();

private slots:
    void setMainScript(int index);
    void onPythonArgsChanged();

    void userChangesChanged();

private:
    PythonProjectRunConfiguration *m_runConfiguration;

    QComboBox *m_fileListCombo;
    QStandardItemModel *m_fileListModel;

    ProjectExplorer::EnvironmentWidget *m_environmentWidget;
};

} // namespace Internal
} // namespace PythonProjectManager

#endif // PYTHONPROJECTRUNCONFIGURATIONWIDGET_H
