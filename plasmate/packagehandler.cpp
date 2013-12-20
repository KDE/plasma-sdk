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
    m_directory = new KDirWatch(this);

    connect(m_directory, &KDirWatch::created, [=](const QString &path) {
        Q_UNUSED(path)
        emit packageChanged(loadPackageInfo());
    });

    connect(m_directory, &KDirWatch::deleted, [=](const QString &path) {
        Q_UNUSED(path)
        emit packageChanged(loadPackageInfo());
    });

    m_fileDefinitions[QStringLiteral("mainscript")] = QStringLiteral("main.qml");
    m_fileDefinitions[QStringLiteral("mainconfigxml")] = QStringLiteral("main.xml");
    m_fileDefinitions[QStringLiteral("configmodel")] = QStringLiteral("config.qml");

    // plasmoid qml
    m_directoryDefinitions.insert(QStringLiteral("ui"), QStringLiteral("mainscript"));
    m_directoryDefinitions.insert(QStringLiteral("config"), QStringLiteral("mainconfigxml"));
    m_directoryDefinitions.insert(QStringLiteral("config"), QStringLiteral("configmodel"));

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
    QString contentsPrefixPaths = m_package.contentsPrefixPaths().at(0);
    if (!contentsPrefixPaths.endsWith(QLatin1Char('/'))) {
        contentsPrefixPaths += QLatin1Char('/');
    }
    return contentsPrefixPaths;
}

void PackageHandler::setPackagePath(const QString &path)
{
    const QString packagePath = QStandardPaths::locate(QStandardPaths::DataLocation, path,
                                                       QStandardPaths::LocateDirectory);
    if (packagePath.isEmpty()) {
        //FIXME missing metadata
        createPackage(path);
    } else {
        if (!m_projectPath.isEmpty()) {
            m_directory->removeDir(m_projectPath);
        }
        setProjectPath(packagePath);
    }

    m_directory->addDir(m_projectPath + contentsPrefix(), KDirWatch::WatchSubDirs | KDirWatch::WatchFiles);

    m_package.setPath(projectPath());
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
    for (const auto &it : m_package.requiredDirectories()) {
        projectDir.mkpath(contentsPrefix() + it);
    }
}

void PackageHandler::createRequiredFiles()
{
    // a package may require a file like ui/main.qml
    // but the ui dir may not be required, so lets check for them
    for (const auto &it : m_package.requiredFiles()) {
        if (m_directoryDefinitions.values().contains(it)) {
            const QString dirName(m_directoryDefinitions.key(it));
            QDir dir(m_projectPath + contentsPrefix());
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

QList<PackageHandler::Node> PackageHandler::loadPackageInfo()
{
    m_nodes.clear();
    QStringList indexedFiles;
    const QString projectPathWithContentsPrefix = m_projectPath + contentsPrefix();

    // TODO it doesn't support unnamed directories like "common"
    // and it doesn't support unnamed directories under
    // named directories like "ui/menu"


    for(const auto &it : m_package.directories()) {
        PackageHandler::Node node;
        node.name = it;
        node.description = m_package.name(it);

        // check for named files like "main.qml" which
        // exist under a named directory like "ui/main.qml"
        if (m_directoryDefinitions.contains(it)) {
            for(const auto &fileIt : m_directoryDefinitions.values()) {
                indexedFiles.append(fileIt.toLocal8Bit().data());
                PackageHandler::Node childNode;
                childNode.name = fileIt;
                childNode.description = m_package.name(fileIt.toLocal8Bit().data());
                indexedFiles.append(childNode.description);
                node.children.append(childNode);
            }
        }

        // check for unnamed files
        for (const auto &fileInfo : QDir(projectPathWithContentsPrefix + it).
                                    entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
            const QString fileName = fileInfo.fileName();
            if (!indexedFiles.contains(fileName)){
                PackageHandler::Node childNode;
                childNode.name = fileName;
                childNode.description = fileName;
                node.children.append(childNode);
            }
        }
        m_nodes.append(node);
    }

    // check for named files which doesn't
    // exist under a named directory
    for(const auto &it : m_package.files()) {
        if (!indexedFiles.contains(it)) {
            PackageHandler::Node node;
            node.name = it;
            node.description = m_package.name(it);
            m_nodes.append(node);
        }
    }

    // there is only one category of files which
    // we might haven't checked. Those are the
    // top level unnamed files like "contents/bar.qml"
    for (const auto &fileInfo : QDir(projectPathWithContentsPrefix).entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
        const QString fileName = fileInfo.fileName();
        if (!indexedFiles.contains(fileName)){
            PackageHandler::Node childNode;
            childNode.name = fileName;
            childNode.description = fileName;
            m_nodes.append(childNode);
        }
    }

    PackageHandler::Node node;
    node.name = QStringLiteral("metadata.desktop");
    node.description = QStringLiteral("metadata.desktop");
    m_nodes.append(node);

    return m_nodes;
}
