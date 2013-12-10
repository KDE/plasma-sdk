/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QUrl>

#include "ui_startpage.h"

class QLabel;
class QComboBox;
class QListWidget;
class QVBoxLayout;
class QModelIndex;
class KPushButton;
class KLineEdit;
class MainWindow;
class ProjectManager;

static const QString PROJECTRC(".plasmateprojectrc");

class StartPage : public QWidget
{
    Q_OBJECT

    enum Roles {
        FullPathRole = Qt::UserRole + 1
    };

    friend class ProjectManager;

public:
    StartPage(MainWindow *parent);
    ~StartPage();

    QString userName();
    QString userEmail();

    bool selectedJsRadioButton();
    bool selectedRbRadioButton();
    bool selectedDeRadioButton();
    bool selectedPyRadioButton();

    void resetStatus();

    enum ProjectTypes {
        Theme           = 1,
        Plasmoid        = 2,
        DataEngine      = 4,
        Runner          = 8
    };

    Q_DECLARE_FLAGS(ProjectType, ProjectTypes)

signals:
    void projectSelected(const QString &name);
public Q_SLOTS:
    void cancelNewProject();

private Q_SLOTS:
    void recentProjectSelected(const QModelIndex &);
    void validateProjectType(const QModelIndex &sender);
    void createNewProject();
    void checkProjectName(const QString &name);
    void checkLocalProjectPath(const QString &path);
    void checkPackagePath(const QString &path);
    void importPackage();
    void doGHNSImport();
    void refreshRecentProjectsList();
    void loadLocalProject();
    void saveNewProjectPreferences(const QString &path);

private:
    enum NewProjectRows {
        PlasmoidRow = 0,
        DataEngineRow = 1,
        RunnerRow = 2,
        ThemeRow = 3,
        WindowSwitcherRow = 4,
        KWinScriptRow = 5,
        KWinEffectRow = 6
    };

    void setupWidgets();
    static QString camelToSnakeCase(const QString& name);
    const QString generateProjectFolderName(const QString& suggestion);
    void selectProject(const QUrl &target);
    void ensureProjectrcFileExists(const QString& projectPath);

    Ui::StartPage m_ui;
    MainWindow *m_parent;

    ProjectManager *m_projectManager;
};

#endif // STARTPAGE_H
