/*

Copyright 2014 Antonis Tsiapaliokas <kok3rs@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "projecthandler.h"

#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QStringList>

#include <KConfigGroup>
#include <KSharedConfig>

ProjectHandler::ProjectHandler(QObject *parent)
    : QObject(parent)
{
}

ProjectHandler::~ProjectHandler()
{
}

const QStringList &ProjectHandler::loadProjectsList()
{
    m_projectsList.clear();
    const QString projectPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);


    KConfigGroup projectsConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    const QStringList recentProjects = projectsConfig.readEntry(QStringLiteral("RecentProjects"), QStringList());
    m_blacklistProjects = projectsConfig.readEntry("blacklistProject", QStringList());
    m_projectsList << recentProjects;

    // load the projects which are inside on our homedir like ~/.local5
    QDir projectDir(projectPath);
    for (const auto &it : projectDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        // TODO find a way to check if the package is valid
        QDir currentProject(projectPath + '/' + it);
        if (currentProject.exists(QStringLiteral("metadata.desktop")) &&
            !currentProject.path().isEmpty() &&
            !m_blacklistProjects.contains(currentProject.path()) &&
            !m_projectsList.contains(currentProject.path()) &&
            !recentProjects.contains(currentProject.path())) {
            m_projectsList << currentProject.path();
        }
    }

    // load the external projects
    QStringList externalProjects = projectsConfig.readEntry("externalProjects", QStringList());
    if (!externalProjects.isEmpty()) {
        for (const auto it : externalProjects) {
            if (!it.isEmpty() && !m_blacklistProjects.contains(it)) {
                m_projectsList << it;
            }
        }
    }

    return m_projectsList;
}

void ProjectHandler::addProject(const QString &projectPath)
{
    QDir projectDir(projectPath);

    if (!projectDir.exists(QStringLiteral("metadata.desktop"))) {
        qWarning() << "the project " << projectPath << "is not valid. metadata.desktop cannot be found";
        return;
    }

    // In order our tests to run. We use qApp->applicationDisplayName()
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    const QString applicationDataPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    // We load the current config and if the external project
    // doesn't exist. Then we are adding it.
    // Also if our project is inside our application data location (usually ~/.local5/plasmate/)
    // we must not add it as an external project.
    QStringList currentConfig = projectConfig.readEntry("externalProjects", QStringList());
    if (!currentConfig.contains(projectPath) && !projectPath.contains(applicationDataPath)) {
        //Here we add our new project inside to our config
        currentConfig << projectPath;
        projectConfig.writeEntry("externalProjects", currentConfig);
        projectConfig.sync();
    }

    if (m_blacklistProjects.contains(projectPath)) {
        whitelistProject(projectPath);
    }

    // Now we are updating the list of projects.
    loadProjectsList();
}

void ProjectHandler::removeProject(const QString &projectPath)
{
    // In order for our tests to run. We use qApp->applicationDisplayName()
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    const QString localProjectPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    const QStringList currentConfig = projectConfig.readEntry("externalProjects", QStringList());
    if (!currentConfig.contains(projectPath) && !projectPath.startsWith(localProjectPath)) {
        qWarning() << "The " << projectPath << " doesn't exist. We cannot remove it";
        return;
    }

    //Here we are removing our project from the config
    const int projectPos = currentConfig.indexOf(projectPath);
    QStringList newConfig = currentConfig;
    newConfig.removeAt(projectPos);

    projectConfig.writeEntry("externalProjects", newConfig);
    projectConfig.sync();

    blacklistProject(projectPath);
    // Now we are updating the list of our projects.
    loadProjectsList();
}

bool ProjectHandler::removeProjectFromDisk(const QString &projectPath)
{
    QDir projectDir(projectPath);
    removeProject(projectPath);
    return projectDir.removeRecursively();
}

void ProjectHandler::blacklistProject(const QString &projectPath)
{
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    QStringList blacklist = projectConfig.readEntry("blacklistProject", QStringList());

    blacklist << projectPath;
    projectConfig.writeEntry("blacklistProject", blacklist);
    projectConfig.sync();
    m_blacklistProjects = blacklist;
}

void ProjectHandler::whitelistProject(const QString &projectPath)
{
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    QStringList blacklist = projectConfig.readEntry("blacklistProject", QStringList());

    const int projectPos = blacklist.indexOf(projectPath);
    if (projectPos == -1) {
        qWarning() << "The " << projectPath << " is not blacklisted, we cannot whitelist it";
        return;
    }

    blacklist.removeAt(projectPos);
    projectConfig.writeEntry("blacklistProject", blacklist);
    projectConfig.sync();

    m_blacklistProjects = blacklist;
}

void ProjectHandler::recentProject(const QString &projectPath)
{
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));
    QStringList recentProjects = projectConfig.readEntry(QStringLiteral("RecentProjects"), QStringList());

    if (recentProjects.isEmpty()) {
        projectConfig.writeEntry(QStringLiteral("RecentProjects"), QStringList() << projectPath);
    } else {
        if (recentProjects.contains(projectPath)) {
            const int projectPos = recentProjects.indexOf(projectPath);
            if (projectPos == -1) {
                qWarning() << "There is something wrong with our project.";
                return;
            }

            recentProjects.removeAt(projectPos);
        }

        recentProjects.insert(0, projectPath);
        projectConfig.writeEntry(QStringLiteral("RecentProjects"), recentProjects);
    }

    projectConfig.sync();
}

