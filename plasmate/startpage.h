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

#include <QUrl>

#include "ui_startpage.h"

class QModelIndex;
//class MainWindow;
class ProjectManager;
class ProjectHandler;
class PackageHandler;

class StartPage : public QWidget
{
    Q_OBJECT

    enum Roles {
        FullPathRole = Qt::UserRole + 1
    };

public:
    //StartPage(MainWindow *parent = 0);
    StartPage(QWidget *parent = 0);
    ~StartPage();

    QString userName();
    QString userEmail();

    bool selectedJsRadioButton();
    bool selectedDeRadioButton();

    void resetStatus();

signals:
    void projectSelected(const QString &name);

private Q_SLOTS:
    void createNewProject();
    void checkLocalProjectPath(const QString &path);
    void refreshRecentProjectsList();

private:
    enum NewProjectRows {
        PlasmoidRow = 0,
        ThemeRow = 1,
        WindowSwitcherRow = 2,
        KWinScriptRow = 3,
        KWinEffectRow = 4
    };

    void setupWidgets();
    static QString camelToSnakeCase(const QString& name);
    const QString generateProjectFolderName(const QString& suggestion);
    void selectProject(const QUrl &target);
    void updateProjectPreferences();

    Ui::StartPage m_ui;
    //MainWindow *m_parent;
    #pragma message("TODO: restore MainWindow when it gets ported")
    QWidget *m_parent;
    ProjectManager *m_projectManager;
    ProjectHandler *m_projectHandler;
    PackageHandler *m_packageHandler;
};

#endif // STARTPAGE_H
