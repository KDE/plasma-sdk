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
#include <QMimeDatabase>

#include "packagehandler.h"
#include "plasmoidhandler.h"

#include "../editors/metadata/metadatahandler.h"

PlasmoidHandler::PlasmoidNode::PlasmoidNode(const QString &name, const QString &description,
                                            const QStringList &mimeTypes, PlasmoidHandler::Node *parent)
    : PackageHandler::Node(name, description, mimeTypes, parent)
{
}

PlasmoidHandler::PlasmoidNode::~PlasmoidNode()
{
   // qDeleteAll(m_childNodes);
}

bool PlasmoidHandler::PlasmoidNode::needsNewFileNode() const
{
    if (name() == QStringLiteral("config") || isFile()) {
        return false;
    }

    return true;
}


PlasmoidHandler::PlasmoidHandler(QObject *parent)
        : PackageHandler(parent),
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

PlasmoidHandler::~PlasmoidHandler()
{
}

void PlasmoidHandler::setPackagePath(const QString &path)
{
    PackageHandler::setPackagePath(path);

    if (QFile(packagePath() + QStringLiteral("metadata.desktop")).exists()) {
        findMainScript();
    }
}

void PlasmoidHandler::findMainScript()
{
    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(packagePath() + QStringLiteral("metadata.desktop"));

    QString mainScript = metadataHandler.mainScript();
    mainScript = mainScript.split(QLatin1Char('/')).last();

    if (mainScript != m_fileDefinitions[QStringLiteral("mainscript")]) {
        m_fileDefinitions[QStringLiteral("mainscript")] = mainScript;
    }
}

QUrl PlasmoidHandler::urlForNode(PackageHandler::Node *node)
{
    if (!node) {
        return QUrl();
    }

    QString path = packagePath() + contentsPrefix();

    if (node->parent()) {
        path += node->parent()->name();
    }

    if (!path.endsWith(QLatin1Char('/'))) {
        path += QLatin1Char('/');
    }

    if (node->isFile() && node->name() != QStringLiteral("New..")) {

        // is it a special file like mainscript?
        if (m_fileDefinitions.contains(node->name())) {
            path += m_fileDefinitions.value(node->name());
        } else {
            path += node->name();
        }

        QFile f(path);
        if (!f.exists()) {
            f.open(QIODevice::ReadWrite);
        }
    }

    return QUrl::fromLocalFile(path);
}

void PlasmoidHandler::createPackage(const QString &userName, const QString &userEmail,
                                   const QString &serviceType, const QString &pluginName)
{
    const QString metadataFilePath = packagePath() + QStringLiteral("metadata.desktop");
    QFile f(metadataFilePath);
    f.open(QIODevice::ReadWrite);

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(metadataFilePath);
    metadataHandler.setPluginApi(QStringLiteral("declarativeappletscript"));
    metadataHandler.setMainScript(QStringLiteral("/ui/main.qml"));
    metadataHandler.writeFile();

    findMainScript();

    PackageHandler::createPackage(userName, userEmail, serviceType, pluginName);

    createRequiredFiles(serviceType, pluginName, userName, userEmail, QStringLiteral(".qml"));
}

void PlasmoidHandler::createRequiredFiles(const QString &serviceType, const QString &pluginName,
                                         const QString &userName, const QString &userEmail, const QString &fileExtension)
{
    // a package may require a file like ui/main.qml
    // but the ui dir may not be required, so lets check for them
    for (const auto &it : package().requiredFiles()) {
        if (m_directoryDefinitions.values().contains(it)) {
            const QString dirName(m_directoryDefinitions.key(it));
            QDir dir(packagePath() + contentsPrefix());
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

PlasmoidHandler::Node* PlasmoidHandler::loadPackageInfo()
{

    package().setPath(packagePath());

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

    const QString packagePathWithContentsPrefix = packagePath() + contentsPrefix();

    // TODO it doesn't support unnamed directories like "common"
    // and it doesn't support unnamed directories under
    // named directories like "ui/menu"


    for(const auto &it : package().directories()) {
        PlasmoidHandler::PlasmoidNode *node = new PlasmoidHandler::PlasmoidNode(it, package().name(it),
                                                                                QStringList(), m_topNode);

        if (node->needsNewFileNode()) {
            QStringList newMimeType;
            if (it == QStringLiteral("images") || it == QStringLiteral("theme")) {
                newMimeType.append(QStringLiteral("[plasmate]/imageDialog"));
            } else {
                newMimeType.append(QStringLiteral("[plasmate]/new"));
            }

            PlasmoidHandler::Node *newNode = new PlasmoidHandler::Node(QStringLiteral("New.."),
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

                const QString description = package().name(fileIt.toLocal8Bit().data());
                QStringList mimeTypes;

                if (fileIt == QStringLiteral("mainconfigxml")) {
                    mimeTypes.append(QStringLiteral("[plasmate]/kconfigxteditor"));
                } else {
                    mimeTypes = mimeTypesForFile(name);
                }

                PlasmoidHandler::Node *childNode = new PlasmoidHandler::Node(fileIt,
                                                  package().name(fileIt.toLocal8Bit().data()),
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

                PlasmoidHandler::Node *childNode = new PlasmoidHandler::Node(fileName, fileName, childMimeTypes, node);
                node->addChild(childNode);
            }
        }
        m_topNode->addChild(node);
    }

    // check for named files which doesn't
    // exist under a named directory
    for(const auto &it : package().files()) {
        if (!indexedFiles.contains(it)) {
            QStringList mimeTypes;
            mimeTypes = package().mimeTypes(it);
            PlasmoidHandler::Node *node = new PlasmoidHandler::Node(it, package().name(it), mimeTypes, m_topNode);
            m_topNode->addChild(node);
        }
    }

    // there is only one category of files which
    // we might haven't checked. Those are the
    // top level unnamed files like "contents/bar.qml"
    for (const auto &fileInfo : QDir(packagePathWithContentsPrefix).entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
        const QString fileName = fileInfo.fileName();
        if (!indexedFiles.contains(fileName)) {
            PlasmoidHandler::Node *node = new PlasmoidHandler::Node(fileName, fileName, mimeTypesForFile(fileName), m_topNode);
            m_topNode->addChild(node);
        }
    }

    QStringList metadataMimeTypes;
    metadataMimeTypes << QStringLiteral("[plasmate]/metadata");
    PlasmoidHandler::Node *node = new PlasmoidHandler::Node(QStringLiteral("metadata.desktop"),
                                 QStringLiteral("metadata.desktop"), metadataMimeTypes, m_topNode);
    m_topNode->addChild(node);

    emit packageChanged(m_topNode);
    return m_topNode;
}

