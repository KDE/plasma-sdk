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

ThemeHandler::ThemeNode::ThemeNode(const QString &name, const QString &description,
                                   const QStringList &mimeTypes, ThemeHandler::Node *parent)
    : PackageHandler::Node(name, description, mimeTypes, parent)
{
}

ThemeHandler::ThemeNode::~ThemeNode()
{
}

ThemeHandler::ThemeHandler(QObject *parent)
        : PackageHandler(parent),
          m_rootNode(nullptr)
{
    QStringList files;
    QStringList dirs;

    auto populateHash = [&]() {
        for (const auto &dir : dirs) {
            for (const auto &file : files) {
                m_entries.insertMulti(dir, file);
            }
        }
        dirs.clear();
        files.clear();
    };

    dirs << QStringLiteral("dialogs")
         << QStringLiteral("opaque/dialogs")
         << QStringLiteral("locolor/dialogs");

    files << QStringLiteral("background")
          << QStringLiteral("shutdowndialog");

    populateHash();

    dirs << QStringLiteral("widgets")
         << QStringLiteral("locolor/widgets");

    files << QStringLiteral("background")
          << QStringLiteral("clock")
          << QStringLiteral("plot-background")
          << QStringLiteral("panel-background")
          << QStringLiteral("tooltip");

    populateHash();

    dirs << QStringLiteral("opaque/widgets");

    files << QStringLiteral("panel-background")
          << QStringLiteral("tooltip");

    populateHash();
}

ThemeHandler::~ThemeHandler()
{
}

void ThemeHandler::setPackagePath(const QString &path)
{
    PackageHandler::setPackagePath(path);
}

QString ThemeHandler::packageType() const
{
    return QStringLiteral("Plasma/Theme");
}

QUrl ThemeHandler::urlForNode(PackageHandler::Node *node)
{
    if (!node) {
        return QUrl();
    }

    QString path = packagePath();

    if (node->name() != QStringLiteral("metadata.desktop")) {
        path += contentsPrefix();
    }

    if (node->parent()->name().isEmpty()) {
        path += node->name();
        return QUrl::fromLocalFile(path);
    }

    QString p = path + node->name();

    if (QFile(p + QStringLiteral(".svg")).exists()) {
        p += QStringLiteral(".svg");
        path = p;
    } else if (QFile(path + QStringLiteral(".svgz")).exists()) {
        p += QStringLiteral(".svgz");
        path = p;
    }

    return QUrl::fromLocalFile(path);
}

void ThemeHandler::createPackage(const QString &userName, const QString &userEmail,
                                 const QString &serviceType, const QString &pluginName)
{
    const QString metadataFilePath = packagePath() + QStringLiteral("metadata.desktop");
    QFile f(metadataFilePath);
    f.open(QIODevice::ReadWrite);

    PackageHandler::createPackage(userName, userEmail, serviceType, pluginName);
}

PackageHandler::Node* ThemeHandler::loadPackageInfo()
{
    package().setPath(packagePath());

    auto findMimetype = [=](const QString &parentName, const QString &childName) {
        QString filePath = packagePath() + contentsPrefix() + parentName;

        if (!filePath.endsWith(QLatin1Char('/'))) {
            filePath += QLatin1Char('/');
        }

        filePath += childName;

        if (QFile(filePath + QStringLiteral("svg")).exists() ||
            QFile(filePath + QStringLiteral("svgz")).exists()) {
            return QStringList() << QStringLiteral("[plasmate]/imageViewer");
        } else {
            QString mimeType = QStringLiteral("[plasmate]/themeImageDialog/");
            mimeType += parentName;
            return QStringList() << mimeType;
        }

        return QStringList();
    };

    m_rootNode = new PackageHandler::Node(QString(), QString(), QStringList());

    for (const auto &rootName : m_entries.uniqueKeys()) {
        const QString description = package().name(qPrintable(rootName));
        PackageHandler::Node *topNode = new PackageHandler::Node(rootName, description,
                                                                 QStringList(), m_rootNode);
        m_rootNode->addChild(topNode);

        for (const auto &childName : m_entries.values(rootName)) {
            const QString name = rootName + QLatin1Char('/') + childName;
            const QString description = package().name(qPrintable(name));
            const QStringList mimeTypes = findMimetype(rootName, name);
            PackageHandler::Node *childNode = new PackageHandler::Node(name, description, mimeTypes, topNode);
            topNode->addChild(childNode);
        }
    }

    PackageHandler::Node *colorsNode = new PackageHandler::Node(QStringLiteral("colors"),
                                                          QStringLiteral("KColorScheme configuration file"),
                                                          QStringList() << QStringLiteral("image/svg+xml"), m_rootNode);

    m_rootNode->addChild(colorsNode);

    QStringList metadataMimeTypes;
    metadataMimeTypes << QStringLiteral("[plasmate]/metadata");
    ThemeHandler::Node *node = new PackageHandler::Node(QStringLiteral("metadata.desktop"),
                                                        QStringLiteral("metadata.desktop"),
                                                        metadataMimeTypes, m_rootNode);
    m_rootNode->addChild(node);

    emit packageChanged(m_rootNode);
    return m_rootNode;
}

