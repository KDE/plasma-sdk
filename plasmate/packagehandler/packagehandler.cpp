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


PackageHandler::Node::Node(const QString &name, const QString &description,
                           const QStringList &mimeTypes, PackageHandler::Node *parent)
    : m_name(name),
      m_description(description),
      m_parent(parent),
      m_mimeTypes(mimeTypes)
{
}

PackageHandler::Node::~Node()
{
    qDeleteAll(m_childNodes);
}

QString PackageHandler::Node::name() const
{
    return m_name;
}

QString PackageHandler::Node::description() const
{
    return m_description;
}

QStringList PackageHandler::Node::mimeTypes() const
{
    return m_mimeTypes;
}

bool PackageHandler::Node::isFile() const
{
    return !m_mimeTypes.isEmpty();
}

QList<PackageHandler::Node*> PackageHandler::Node::childNodes() const
{
    return m_childNodes;
}

PackageHandler::Node *PackageHandler::Node::parent() const
{
    return m_parent;
}

PackageHandler::Node *PackageHandler::Node::child(int row) const
{
    return m_childNodes.value(row);
}

void PackageHandler::Node::addChild(PackageHandler::Node *child)
{
    m_childNodes.append(child);
}

int PackageHandler::Node::row()
{
    if (m_parent) {
        return m_parent->childNodes().indexOf(static_cast<PackageHandler::Node*>(this));
    }
    return 0;
}


PackageHandler::PackageHandler(QObject *parent)
        : QObject(parent)
{
}

PackageHandler::~PackageHandler()
{
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
    if (!path.endsWith(QLatin1Char('/'))) {
        m_packagePath = path + QLatin1Char('/');
    } else {
        m_packagePath = path;
    }

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(m_packagePath + QLatin1Char('/') + QStringLiteral("metadata.desktop"));

    m_package = Plasma::PluginLoader::self()->loadPackage(metadataHandler.serviceTypes().at(0));;
}

void PackageHandler::createPackage(const QString &userName, const QString &userEmail,
                                   const QString &serviceType, const QString &pluginName)
{
    QDir dir;
    dir.mkpath(m_packagePath);
    dir.cd(m_packagePath);

    const QString metadataFilePath = m_packagePath + QStringLiteral("metadata.desktop");
    QFile f(metadataFilePath);
    f.open(QIODevice::ReadWrite);

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(metadataFilePath);
    metadataHandler.setName(pluginName);
    metadataHandler.setServiceTypes(QStringList() << serviceType);
    metadataHandler.setVersion(QChar(1));
    metadataHandler.setAuthor(userName);
    metadataHandler.setEmail(userEmail);
    metadataHandler.setLicense(QStringLiteral("GPL"));
    metadataHandler.writeFile();

    createRequiredDirectories();
}

void PackageHandler::createRequiredDirectories()
{
    QDir projectDir(m_packagePath);
    for (const auto &it : m_package.requiredDirectories()) {
        projectDir.mkpath(contentsPrefix() + it);
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

