/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "thememodel.h"
#include "themelistmodel.h"
#include <QDebug>
#include <QByteArray>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QIcon>
#include <QStandardPaths>

#include <QXmlSimpleReader>
#include <QXmlDefaultHandler>
#include <QXmlInputSource>
#include <QXmlInputSource>

#include <KCompressionDevice>
#include <KProcess>
#include <KIO/Job>

#include <Plasma/Theme>

class IconsParserHandler : public QXmlDefaultHandler
{
public:
    IconsParserHandler();
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &atts) Q_DECL_OVERRIDE;
    QStringList m_ids;
};

IconsParserHandler::IconsParserHandler()
    : QXmlDefaultHandler()
{}

bool IconsParserHandler::startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &atts)
{
    const QString id = atts.value("id");
    //qWarning() << "Start Element:"<<id;

    if (!id.isEmpty() && !id.contains(QRegExp("\\d\\d$")) &&
        id != "base" && !id.contains("layer")) {
        m_ids<<id;
    }
    return true;
}

ThemeModel::ThemeModel(const KPackage::Package &package, QObject *parent)
    : QAbstractListModel(parent),
      m_theme(new Plasma::Theme),
      m_themeName(QStringLiteral("default")),
      m_package(package),
      m_themeListModel(new ThemeListModel(this))
{
    m_theme->setUseGlobalSettings(false);
    m_theme->setThemeName(m_themeName);

    m_roleNames.insert(ImagePath, "imagePath");
    m_roleNames.insert(Description, "description");
    m_roleNames.insert(Delegate, "delegate");
    m_roleNames.insert(UsesFallback, "usesFallback");
    m_roleNames.insert(SvgAbsolutePath, "svgAbsolutePath");
    m_roleNames.insert(IsWritable, "isWritable");
    m_roleNames.insert(IconElements, "iconElements");

    load();
}

ThemeModel::~ThemeModel()
{
}

ThemeListModel *ThemeModel::themeList()
{
    return m_themeListModel;
}

QHash<int, QByteArray> ThemeModel::roleNames() const
{
    return m_roleNames;
}

int ThemeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_jsonDoc.array().size();
}

QVariant ThemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() > m_jsonDoc.array().size()) {
        return QVariant();
    }

    const QVariantMap value = m_jsonDoc.array().at(index.row()).toObject().toVariantMap();

    switch (role) {
    case ImagePath:
        return value.value("imagePath");
    case Description:
        return value.value("description");
    case Delegate:
        return value.value("delegate");
    case UsesFallback:
        return !m_theme->currentThemeHasImage(value.value("imagePath").toString());
    case SvgAbsolutePath: {
        QString path = m_theme->imagePath(value.value("imagePath").toString());
        if (!value.value("imagePath").toString().contains("translucent")) {
             path = path.replace("translucent/", "");
        }
        return path;
    }
    case IsWritable:
        return QFile::exists(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/desktoptheme/" + m_themeName);
    case IconElements: {
        QString path = m_theme->imagePath(value.value("imagePath").toString());
        if (!value.value("imagePath").toString().contains("translucent")) {
             path = path.replace("translucent/", "");
        }
        KCompressionDevice file(path, KCompressionDevice::GZip);
        if (!file.open(QIODevice::ReadOnly)) {
            return QVariant();
        }

        QXmlSimpleReader reader;
        IconsParserHandler handler;
        reader.setContentHandler(&handler);
        QXmlInputSource source(&file);
        reader.parse(&source);
        qWarning()<<handler.m_ids;
        return handler.m_ids;
    }
    }

    return QVariant();
}




void ThemeModel::load()
{
    beginResetModel();
    qDebug() << "Loading theme description file" << m_package.filePath("data", "themeDescription.json");

    QFile jsonFile(m_package.filePath("data", "themeDescription.json"));
    jsonFile.open(QIODevice::ReadOnly);

    QJsonParseError error;
    m_jsonDoc = QJsonDocument::fromJson(jsonFile.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing Json" << error.errorString();
    }

    endResetModel();
}

QString ThemeModel::theme() const
{
    return m_themeName;
}

void ThemeModel::setTheme(const QString& theme)
{
    if (theme == m_themeName) {
        return;
    }

    m_themeName = theme;
    m_theme->setThemeName(theme);
    load();
    emit themeChanged();
}

void ThemeModel::editElement(const QString& imagePath)
{
    QString file = m_theme->imagePath(imagePath);
    if (!file.contains("translucent")) {
            file = file.replace("translucent/", "");
    }

    QString finalFile;

    if (m_theme->currentThemeHasImage(imagePath)) {
        finalFile = file;
    } else {
        finalFile = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/desktoptheme/" + m_themeName + "/" + imagePath + ".svgz";
        KIO::FileCopyJob *job = KIO::file_copy( QUrl::fromLocalFile(file), QUrl::fromLocalFile(finalFile) );
        if (!job->exec()) {
            qWarning() << "Error copying" << file << "to" << finalFile;
        }
    }

    //QProcess::startDetached("inkscape", QStringList() << finalFile);
    KProcess *process = new KProcess();
    //TODO: don't use the script to not depend from bash/linux?
    process->setProgram("bash", QStringList() << m_package.filePath("scripts", "openInEditor.sh") << finalFile);

    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished()));
    process->start();
}

void ThemeModel::processFinished()
{
    qWarning()<<sender();
    

    /*We increment the microversion of the theme: keeps track and will force the cache to be
      discarded in order to reload immediately the graphics*/
    const QString metadataPath(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1Literal("plasma/desktoptheme/") % m_themeName % QLatin1Literal("/metadata.desktop")));
    KConfig c(metadataPath);
    KConfigGroup cg(&c, "Desktop Entry");

    QStringList version = cg.readEntry("X-KDE-PluginInfo-Version", "0.0").split('.');
    if (version.length() < 2) {
        version << QLatin1Literal("0");
    }
    if (version.length() < 3) {
        version << QLatin1Literal("0");
    }

    cg.writeEntry("X-KDE-PluginInfo-Version", QString(version.first() + QLatin1Literal(".") + version[1] + QLatin1Literal(".") + QString::number(version.last().toInt() + 1)));
    cg.sync();
}

#include "moc_thememodel.cpp"
