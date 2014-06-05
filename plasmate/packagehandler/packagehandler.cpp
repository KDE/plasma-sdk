/*

Copyright 2013 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@Kde.org>
Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@Kde.org>

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


#include <Plasma/PluginLoader>

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QStandardPaths>

#include "packagehandler.h"
#include "../editors/metadata/metadatahandler.h"

PackageHandler::PackageHandler(QObject *parent)
    : QObject(parent)
{
}

PackageHandler::~PackageHandler()
{
}

QString PackageHandler::contentsPrefix() const
{
    if (package().contentsPrefixPaths().isEmpty()) {
        return QString();
    }

    QString contentsPrefixPaths = package().contentsPrefixPaths().at(0);

    if (contentsPrefixPaths.isEmpty()) {
        return QString();
    }

    if (!contentsPrefixPaths.endsWith(QLatin1Char('/'))) {
        contentsPrefixPaths += QLatin1Char('/');
    }
    return contentsPrefixPaths;
}

void PackageHandler::setPackagePath(const QString &path)
{
    if (!path.endsWith(QLatin1Char('/'))) {
        m_packagePath = path + QLatin1Char('/');
    } else {
        m_packagePath = path;
    }

    // create the dir if it doesn't exist
    if (!QDir(m_packagePath).exists()) {
        QDir dir;
        dir.mkpath(m_packagePath);
    }

    if (QFile(m_packagePath + QStringLiteral("metadata.desktop")).exists()) {
        preparePackage();
        loadPackage();
    }
}

void PackageHandler::preparePackage()
{
    m_package = Plasma::PluginLoader::self()->loadPackage(packageType());
}

void PackageHandler::createPackage(const QString &userName, const QString &userEmail,
                                   const QString &serviceType, const QString &pluginName)
{
    QDir().mkpath(packagePath());

    const QString metadataFilePath = m_packagePath + QStringLiteral("metadata.desktop");
    preparePackage();

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(metadataFilePath);
    metadataHandler.setName(pluginName);
    metadataHandler.setServiceTypes(QStringList() << serviceType);
    metadataHandler.setVersion(QChar(1));
    metadataHandler.setAuthor(userName);
    metadataHandler.setEmail(userEmail);
    metadataHandler.setLicense(QStringLiteral("GPL"));
    metadataHandler.writeFile();

    preparePackage();

    createRequiredDirectories();
}

void PackageHandler::createRequiredDirectories()
{
    QDir().mkpath(packagePath() + contentsPrefix());

    QDir projectDir(m_packagePath);
    for (const auto &it : m_package.requiredDirectories()) {
        QString dirName;
        if (!contentsPrefix().isEmpty()) {
            dirName = contentsPrefix() + it;
        } else {
            dirName = QLatin1Char('/') + it;
        }

        projectDir.mkpath(dirName);
    }
}

QString PackageHandler::packagePath() const
{
    return m_packagePath;
}

Plasma::Package PackageHandler::package() const
{
    return m_package;
}

void PackageHandler::loadPackage()
{
    m_package.setPath(m_packagePath);
}

