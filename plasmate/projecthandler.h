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

#ifndef PROJECTHANDLER_H
#define PROJECTHANDLER_H

#include <QObject>
#include <QStringList>

class PackageHandler;

class ProjectHandler : public QObject
{
    Q_OBJECT

public:
    explicit ProjectHandler(QObject *parent = 0);
    ~ProjectHandler();

    const QStringList &loadProjectsList();
    void addProject(const QString &projectPath);
    void removeProject(const QString &projectPath);
    bool removeProjectFromDisk(const QString &projectPath);
    PackageHandler *packageHandler();
private:
    void blacklistProject(const QString &projectPath);
    void whitelistProject(const QString &projectPath);

    QStringList m_projectsList;
    QStringList m_blacklistProjects;
    PackageHandler *m_packageHandler;
};

#endif

