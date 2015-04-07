/*

Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

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

#include <KDirWatch>

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QStandardPaths>

#include "themehandler.h"

#include "../editors/metadata/metadatahandler.h"

ThemeHandler::ThemeHandler(QObject *parent)
    : PackageHandler(parent)
{
   m_orphanDirs[QStringLiteral("opaque")] = QStringLiteral("Opaque Images");
   m_orphanDirs[QStringLiteral("loclor")] = QStringLiteral("Low Color Images");
}

ThemeHandler::~ThemeHandler()
{
}

QString ThemeHandler::packageType() const
{
    return QStringLiteral("Plasma/Theme");
}

void ThemeHandler::createPackage(const QString &userName, const QString &userEmail,
                                 const QString &serviceType, const QString &pluginName)
{
    const QString metadataFilePath = packagePath() + QStringLiteral("metadata.desktop");
    QFile f(metadataFilePath);
    f.open(QIODevice::ReadWrite);

    PackageHandler::createPackage(userName, userEmail, serviceType, pluginName);
    loadPackage();

    createDirectories();
}

QString ThemeHandler::description(const QString &entry) const
{
    const QString description = m_orphanDirs.value(entry);
    if (!description.isEmpty()) {
        return description;
    }

    return package().name(entry.toLocal8Bit().data());
}

void ThemeHandler::createDirectories()
{
    QDir projectDir(packagePath());
    for (const auto &it : package().directories()) {
        projectDir.mkpath(it);
    }
}

