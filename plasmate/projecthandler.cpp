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
#include "packagehandler.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QStringList>

#include <KConfigGroup>
#include <KSharedConfig>

ProjectHandler::ProjectHandler(QObject *parent)
    : QObject(parent),
      m_packageHandler(new PackageHandler(this))
{
}

ProjectHandler::~ProjectHandler()
{
}

PackageHandler *ProjectHandler::packageHandler()
{
    return m_packageHandler;
}

const QStringList &ProjectHandler::loadProjectsList()
{
    m_projectsList.clear();
    const QString projectPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    // load the projects which are inside on our homedir like ~/.local5
    QDir projectDir(projectPath);
    for (const auto &it : projectDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        // TODO find a way to check if the package is valid
        QDir currentProject(projectPath + '/' + it);
        if (currentProject.exists(QStringLiteral("metadata.desktop")) && !currentProject.path().isEmpty()) {
            m_projectsList << currentProject.path();
        }
    }

    // load the external projects
    KConfigGroup externalProjectsConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));

    QString externalProjects = externalProjectsConfig.readEntry("externalProjects", "");

    if (!externalProjects.isEmpty()) {
        for (const auto it : externalProjects.split(',')) {
            if (!it.isEmpty()) {
                m_projectsList << it;
            }
        }
    }

    return m_projectsList;
}

void ProjectHandler::addProject(const QString &projectPath)
{
    QDir projectDir(projectPath);

    if (!projectDir.exists()) {
        // If our package doesn't exist we are creating it.
        m_packageHandler->setPackagePath(projectDir.absolutePath());
    } else  if (!projectDir.exists(QStringLiteral("metadata.desktop"))) {
        qWarning() << "the project " << projectPath << "is not valid. metadata.desktop cannot be found";
        return;
    }

    // In order our tests to run. We use qApp->applicationDisplayName()
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));

    // We load the current config and if the external project
    // doesn't exist. Then we are adding it.
    const QString currentConfig = projectConfig.readEntry("externalProjects", "");
    if (!currentConfig.contains(projectPath)) {
        const QString updatedConfig = currentConfig + ',' + projectPath;
        projectConfig.writeEntry("externalProjects", updatedConfig);
        projectConfig.sync();
    }

    // Now we are updating the list of projects.
    loadProjectsList();
}

void ProjectHandler::removeProject(const QString &projectPath)
{
    // In order for our tests to run. We use qApp->applicationDisplayName()
    KConfigGroup projectConfig(KSharedConfig::openConfig(qApp->applicationDisplayName()), QStringLiteral("ProjectHandler"));

    QString currentConfig = projectConfig.readEntry("externalProjects", "");
    if (!currentConfig.contains(projectPath)) {
        qWarning() << "The " << projectPath << " doesn't exist. We cannot remove it";
        return;
    }

    // In order to remove the project we are replacing the project path
    // and the ',' which appends before it.
    const QString newConfig = currentConfig.replace(',' + projectPath, "");
    projectConfig.writeEntry("externalProjects", newConfig);
    projectConfig.sync();

    // Now we are updating the list of our projects.
    loadProjectsList();
}

