/*

Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>

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


#include "packagehandler.h"

#include <Plasma/PluginLoader>

#include <KDirWatch>

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>

PackageHandler::PackageHandler(QObject *parent)
        : QObject(parent),
          m_directory(0)
{
    m_fileDefinitions[QStringLiteral("mainscript")] = QStringLiteral("main.qml");
    m_fileDefinitions[QStringLiteral("mainconfigxml")] = QStringLiteral("main.xml");

    // plasmoid qml
    QStringList plasmoidQmlList;
    plasmoidQmlList << QStringLiteral("mainscript");
    m_directoryDefinitions[QStringLiteral("ui")] = plasmoidQmlList;
    plasmoidQmlList.clear();

    plasmoidQmlList << QStringLiteral("mainconfigxml");
    m_directoryDefinitions[QStringLiteral("config")] = plasmoidQmlList;
}

PackageHandler::~PackageHandler()
{
    delete m_directory;
}

void PackageHandler::setPackageType(const QString &type)
{
    m_package = Plasma::PluginLoader::self()->loadPackage(type);
}

QString PackageHandler::contentsPrefix() const
{
    return m_package.contentsPrefixPaths().at(0);
}

void PackageHandler::setPackagePath(const QString &path)
{
    const QString packagePath = QStandardPaths::locate(QStandardPaths::DataLocation, path,
                                                       QStandardPaths::LocateDirectory);
    if (packagePath.isEmpty()) {
        //FIXME missing metadata
        createPackage(path);
    }
    m_package.setPath(path);
}

void PackageHandler::createPackage(const QString &path)
{
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0));
    dir.mkpath(path);
    dir.cd(path);

    setProjectPath(dir.path());

    const QString metadataFilePath = projectPath() + QStringLiteral("metadata.desktop");
    QFile f(metadataFilePath);
    f.open(QIODevice::ReadWrite);
    // FIXME write the metadata now

    createRequiredDirectories();
    createRequiredFiles();
}

QString PackageHandler::packagePath() const
{
    return m_packagePath;
}

void PackageHandler::setProjectPath(const QString &path)
{
    if (!path.endsWith(QLatin1Char('/'))) {
        m_projectPath = path + QLatin1Char('/');
    } else {
        m_projectPath = path;
    }
}

QString PackageHandler::projectPath() const
{
    return m_projectPath;
}

void PackageHandler::createRequiredDirectories()
{
    QDir projectDir(m_projectPath);
    for (auto it : m_package.requiredDirectories()) {
        projectDir.mkpath(it);
        if (m_directoryDefinitions.contains(it)) {

        }
    }
}

void PackageHandler::createRequiredFiles()
{
    // a package may require a file like ui/main.qml
    // but the ui dir may not be required, so lets check for them
    for (auto it : m_package.requiredFiles()) {
        if (m_directoryDefinitions.values().contains(it)) {
            QStringList l;
            l << it;
            const QString dirName(m_directoryDefinitions.keys(l).at(0));
            QDir dir(m_projectPath);
            dir.mkpath(dirName);
            dir.cd(dirName);

            QFile f;
            const QString filePath = dir.path() + QLatin1Char('/') +
                                     m_fileDefinitions[it];

            if (it == QLatin1String("mainscript")) {
                f.setFileName(QStandardPaths::locate(QStandardPaths::DataLocation,
                                    QStringLiteral("templates/mainPlasmoid.qml")));
                const bool ok = f.copy(filePath);
                if (!ok) {
                    emit error(QStringLiteral("The mainscript file hasn't been created"));
                }
            }

            f.open(QIODevice::ReadWrite);
        }
    }
}

