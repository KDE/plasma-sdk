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


#include <Plasma/PluginLoader>

#include <KDirWatch>

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QStandardPaths>
#include <QMimeDatabase>

#include "packagehandler.h"
#include "editors/metadata/metadatahandler.h"

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

bool PackageHandler::Node::needsNewFileNode() const
{
    if (name() == QStringLiteral("config") || isFile()) {
        return false;
    }

    return true;
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
        : QObject(parent),
          m_topNode(0)
{
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
}

void PackageHandler::loadPackage()
{
    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(m_packagePath + QLatin1Char('/') + QStringLiteral("metadata.desktop"));
    metadataHandler.serviceTypes().at(0);

    m_package = Plasma::PluginLoader::self()->loadPackage(metadataHandler.serviceTypes().at(0));;
    m_package.setPath(m_packagePath);
}

QUrl PackageHandler::urlForNode(PackageHandler::Node *node) const
{
    if (!node) {
        return QUrl();
    }

    QString path = m_packagePath + contentsPrefix();

    if (node->parent()) {
        path += node->parent()->name();
    }

    if (!path.endsWith(QLatin1Char('/'))) {
        path += QLatin1Char('/');
    }

    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << dir.mkpath(path);
    }

    if (node->isFile() && node->name() != QStringLiteral("New..")) {
        path += node->name();
        QFile f(path);
        if (!f.exists()) {
            f.open(QIODevice::ReadWrite);
        }
    }

    return QUrl::fromLocalFile(path);
}

QString PackageHandler::packagePath() const
{
    return m_packagePath;
}

void PackageHandler::createPackage(const QString &userName, const QString &userEmail,
                                   const QString &serviceType, const QString &pluginName,
                                   const QString &mainScriptName, const QString &api, const QString &fileExtension)
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
    metadataHandler.setPluginApi(api);
    metadataHandler.setMainScript(QStringLiteral("/ui/") + mainScriptName);
    metadataHandler.writeFile();

    QString mainScript = metadataHandler.mainScript();
    mainScript = mainScript.split(QLatin1Char('/')).last();
    if (mainScript != m_fileDefinitions[QStringLiteral("mainscript")]) {
        m_fileDefinitions[QStringLiteral("mainscript")] = mainScript;
    }

    m_package = Plasma::PluginLoader::self()->loadPackage(metadataHandler.serviceTypes().at(0));;
    m_package.setPath(m_packagePath);
    createRequiredDirectories();
    createRequiredFiles(serviceType, pluginName, userName, userEmail, fileExtension);
}

void PackageHandler::createRequiredDirectories()
{
    QDir projectDir(m_packagePath);
    for (const auto &it : m_package.requiredDirectories()) {
        projectDir.mkpath(contentsPrefix() + it);
    }
}

void PackageHandler::createRequiredFiles(const QString &serviceType, const QString &pluginName,
                                         const QString &userName, const QString &userEmail, const QString &fileExtension)
{
    // a package may require a file like ui/main.qml
    // but the ui dir may not be required, so lets check for them
    for (const auto &it : m_package.requiredFiles()) {
        if (m_directoryDefinitions.values().contains(it)) {
            const QString dirName(m_directoryDefinitions.key(it));
            QDir dir(m_packagePath + contentsPrefix());
            dir.mkpath(dirName);
            dir.cd(dirName);

            QFile f;
            const QString filePath = dir.path() + QLatin1Char('/') +
                                     m_fileDefinitions[it];
            QString templateFilePath;

            if (it == QLatin1String("mainscript")) {
                if (serviceType == "Plasma/Applet") {
                    templateFilePath.append("mainPlasmoid");
                } else if (serviceType == "KWin/WindowSwitcher") {
                    templateFilePath.append("mainTabbox");
                } else if (serviceType == "KWin/Script") {
                    templateFilePath.append("mainKWinScript");
                } else if (serviceType == "KWin/Effect") {
                    templateFilePath.append("mainKWinEffect");
                }

                f.setFileName(QStandardPaths::locate(QStandardPaths::DataLocation,
                                    QStringLiteral("templates/") + templateFilePath + fileExtension));
                const bool ok = f.copy(filePath);
                if (!ok) {
                    emit error(QStringLiteral("The mainscript file hasn't been created"));
                }

                f.setFileName(filePath);
                f.open(QIODevice::ReadWrite);

                // Now open these files, and substitute the main class, author, email and date fields
                QByteArray tmpPluginName(pluginName.toLocal8Bit());
                QByteArray tmpAuthor(userName.toLocal8Bit());
                QByteArray tmpEmail(userEmail.toLocal8Bit());
                QByteArray rawData = f.readAll();
                f.close();
                f.open(QIODevice::WriteOnly);

                QByteArray replacedString("$PLASMOID_NAME");
                if (rawData.contains(replacedString)) {
                    rawData.replace(replacedString, tmpPluginName);
                }
                replacedString.clear();

                replacedString.append("$AUTHOR");
                if (rawData.contains(replacedString)) {
                    rawData.replace(replacedString, tmpAuthor);
                }
                replacedString.clear();

                replacedString.append("$EMAIL");
                if (rawData.contains(replacedString)) {
                    rawData.replace(replacedString, tmpEmail);
                }
                replacedString.clear();

                replacedString.append("$DATE");
                QDate date = QDate::currentDate();
                QByteArray datetime(date.toString().toUtf8());
                QTime time = QTime::currentTime();
                datetime.append(", " + time.toString().toUtf8());
                if (rawData.contains(replacedString)) {
                    rawData.replace(replacedString, datetime);
                }

                f.write(rawData);
                f.close();
            }

        }
    }
}

PackageHandler::Node* PackageHandler::loadPackageInfo()
{
    auto mimeTypesForFile = [](const QString &fileName) {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(fileName);
        QStringList mimetypes;
        return mimetypes << mime.name();
    };

    if (m_topNode) {
        delete m_topNode;
        m_topNode = 0;
    }

    m_topNode = new PackageHandler::Node(QString(), QString(), QStringList(), m_topNode);

    QStringList indexedFiles;

    // metadata.desktop is a special file we will handle it manually.
    // check in the end of the method
    indexedFiles.append(QStringLiteral("metadata.desktop"));

    const QString packagePathWithContentsPrefix = m_packagePath + contentsPrefix();

    // TODO it doesn't support unnamed directories like "common"
    // and it doesn't support unnamed directories under
    // named directories like "ui/menu"


    for(const auto &it : m_package.directories()) {
        PackageHandler::Node *node = new PackageHandler::Node(it, m_package.name(it),
                                     QStringList(), m_topNode);

        if (node->needsNewFileNode()) {
            QStringList newMimeType;
            if (it == QStringLiteral("images") || it == QStringLiteral("theme")) {
                newMimeType.append(QStringLiteral("[plasmate]/imageDialog"));
            } else {
                newMimeType.append(QStringLiteral("[plasmate]/new"));
            }

            PackageHandler::Node *newNode = new PackageHandler::Node(QStringLiteral("New.."),
                                            QStringLiteral("New.."), newMimeType, node);
            node->addChild(newNode);
        }

        // check for named files like "main.qml" which
        // exist under a named directory like "ui/main.qml"
        if (m_directoryDefinitions.contains(it)) {
            for(const auto &fileIt : m_directoryDefinitions.values(it)) {
                const QString name = m_fileDefinitions.value(fileIt);

                if (name.isEmpty()) {
                    qWarning("Something is wrong with the file definitions");
                }

                const QString description = m_package.name(fileIt.toLocal8Bit().data());
                QStringList mimeTypes;

                if (fileIt == QStringLiteral("mainconfigxml")) {
                    mimeTypes.append(QStringLiteral("[plasmate]/kconfigxteditor"));
                } else {
                    mimeTypes = mimeTypesForFile(name);
                }

                PackageHandler::Node *childNode = new PackageHandler::Node(fileIt,
                                                  m_package.name(fileIt.toLocal8Bit().data()),
                                                  mimeTypes, node);

                indexedFiles << fileIt << description << name;
                node->addChild(childNode);
            }
        }
        // check for unnamed files
        for (const auto &fileInfo : QDir(packagePathWithContentsPrefix + it).
                                    entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
            const QString fileName = fileInfo.fileName();
            if (!indexedFiles.contains(fileName)) {
                QStringList childMimeTypes;

                if (it == QStringLiteral("images") || it == QStringLiteral("theme")) {
                    childMimeTypes.append(QStringLiteral("[plasmate]/imageViewer"));
                } else {
                    childMimeTypes = mimeTypesForFile(fileName);
                }

                PackageHandler::Node *childNode = new PackageHandler::Node(fileName, fileName, childMimeTypes, node);
                node->addChild(childNode);
            }
        }
        m_topNode->addChild(node);
    }

    // check for named files which doesn't
    // exist under a named directory
    for(const auto &it : m_package.files()) {
        if (!indexedFiles.contains(it)) {
            QStringList mimeTypes;
            mimeTypes = m_package.mimeTypes(it);
            PackageHandler::Node *node = new PackageHandler::Node(it, m_package.name(it), mimeTypes, m_topNode);
            m_topNode->addChild(node);
        }
    }

    // there is only one category of files which
    // we might haven't checked. Those are the
    // top level unnamed files like "contents/bar.qml"
    for (const auto &fileInfo : QDir(packagePathWithContentsPrefix).entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
        const QString fileName = fileInfo.fileName();
        if (!indexedFiles.contains(fileName)) {
            PackageHandler::Node *node = new PackageHandler::Node(fileName, fileName, mimeTypesForFile(fileName), m_topNode);
            m_topNode->addChild(node);
        }
    }

    QStringList metadataMimeTypes;
    metadataMimeTypes << QStringLiteral("[plasmate]/metadata");
    PackageHandler::Node *node = new PackageHandler::Node(QStringLiteral("metadata.desktop"),
                                 QStringLiteral("metadata.desktop"), metadataMimeTypes, m_topNode);
    m_topNode->addChild(node);

    emit packageChanged(m_topNode);
    return m_topNode;
}

