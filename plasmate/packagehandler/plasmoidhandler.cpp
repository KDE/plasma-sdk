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

PlasmoidHandler::PlasmoidHandler(QObject *parent)
    : PackageHandler(parent)
{
    m_fileDefinitions[QStringLiteral("mainscript")] = QStringLiteral("main.qml");
    m_fileDefinitions[QStringLiteral("mainconfigxml")] = QStringLiteral("main.xml");
    m_fileDefinitions[QStringLiteral("configmodel")] = QStringLiteral("config.qml");

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

    loadPackage();
}

QString PlasmoidHandler::packageType() const
{
    return QStringLiteral("Plasma/Applet");
}

QString PlasmoidHandler::description(const QString &entry) const
{
    const QString key = m_fileDefinitions.key(entry);

    if (key.isEmpty()) {
        for (const auto dir : package().directories()) {
            if (qstrcmp(entry.toLocal8Bit().data(), dir) == 0) {
                return package().name(entry.toLocal8Bit().data());
            }
        }
    } else {
        return package().name(key.toLocal8Bit().data());
    }

    return QString();
}

