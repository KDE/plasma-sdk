/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "thememodel.h"
#include "coloreditor.h"
#include "themelistmodel.h"
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QIcon>
#include <QStandardPaths>
#include <QXmlStreamReader>

#include <KAboutData>
#include <KCompressionDevice>
#include <KIO/FileCopyJob>
#include <KIO/MkdirJob>
#include <KProcess>

#include <Plasma/Theme>
#include <kio/mkdirjob.h>

using namespace Qt::StringLiterals;

ThemeModel::ThemeModel()
    : QAbstractListModel(nullptr)
    , m_theme(new Plasma::Theme)
    , m_themeName(QStringLiteral("default"))
    , m_imageSet(m_themeName, "plasma/desktoptheme")
    , m_themeListModel(new ThemeListModel(this))
    , m_colorEditor(new ColorEditor(this))
{
    m_theme->setUseGlobalSettings(false);
    m_theme->setThemeName(m_themeName);

    m_roleNames.insert(ImagePath, "imagePath");
    m_roleNames.insert(Description, "description");
    m_roleNames.insert(Delegate, "delegate");
    m_roleNames.insert(UsesFallback, "usesFallback");
    m_roleNames.insert(SvgAbsolutePath, "svgAbsolutePath");
    m_roleNames.insert(IconElements, "iconElements");
    m_roleNames.insert(FrameSvgPrefixes, "frameSvgPrefixes");

    load();
}

QString compactName(const QString &name)
{
    auto compactName = name.toLower();
    compactName.replace(' ', QString());
    return compactName;
}

ThemeListModel *ThemeModel::themeList()
{
    return m_themeListModel;
}

ColorEditor *ThemeModel::colorEditor()
{
    return m_colorEditor;
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
        return !m_imageSet.currentImageSetHasImage(value.value("imagePath").toString());
    case SvgAbsolutePath: {
        QString path = m_imageSet.imagePath(value.value("imagePath").toString());
        if (!value.value("imagePath").toString().contains("translucent")) {
            path = path.replace("translucent/", "");
        }
        return path;
    }
    case IconElements:
    case FrameSvgPrefixes: {
        QString path = m_imageSet.imagePath(value.value("imagePath").toString());
        if (!value.value("imagePath").toString().contains("translucent")) {
            path = path.replace("translucent/", "");
        }
        KCompressionDevice file(path, KCompressionDevice::GZip);
        if (!file.open(QIODevice::ReadOnly)) {
            return QVariant();
        }

        QStringList ids, prefixes;
        QXmlStreamReader reader(&file);
        while (!reader.atEnd() && !reader.hasError()) {
            const auto token = reader.readNext();
            if (token != QXmlStreamReader::StartElement) {
                continue;
            }

            const QString id = reader.attributes().value("id").toString();
            if (!id.isEmpty() && !id.contains(QRegularExpression("\\d\\d$")) && id != "base" && !id.contains("layer")) {
                ids << id;
            }
            if (id.endsWith(QLatin1String("-center")) && !id.contains("hint-")) {
                // remove -center
                prefixes << id.mid(0, id.length() - 7);
            }
        }

        if (role == IconElements) {
            return ids;
        } else {
            return prefixes;
        }
    }
    }

    return QVariant();
}

void ThemeModel::load()
{
    beginResetModel();

    QFile jsonFile(":/qt/qml/org/kde/plasma/themeexplorer/themeDescription.json");
    (void)jsonFile.open(QIODevice::ReadOnly);

    QJsonParseError error;
    m_jsonDoc = QJsonDocument::fromJson(jsonFile.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing Json" << error.errorString();
    }

    endResetModel();
    Q_EMIT isWritableChanged();
}

QString ThemeModel::theme() const
{
    return m_themeName;
}

QString ThemeModel::author() const
{
    const QList<KAboutPerson> authors = m_theme->metadata().authors();
    return authors.isEmpty() ? QString() : authors.at(0).name();
}

QString ThemeModel::email() const
{
    const QList<KAboutPerson> authors = m_theme->metadata().authors();
    return authors.isEmpty() ? QString() : authors.at(0).emailAddress();
}

QString ThemeModel::license() const
{
    return m_theme->metadata().license();
}

QString ThemeModel::website() const
{
    return m_theme->metadata().website();
}

void ThemeModel::setTheme(const QString &theme)
{
    if (theme == m_themeName) {
        return;
    }

    m_themeName = theme;
    m_theme->setThemeName(theme);
    m_imageSet.setImageSetName(theme);
    load();
    m_colorEditor->setTheme(theme);
    emit themeChanged();
}

void ThemeModel::editElement(const QString &imagePath)
{
    QString file = m_imageSet.imagePath(imagePath);
    if (!file.contains("translucent")) {
        file = file.replace("translucent/", "");
    }

    QString finalFile;

    if (m_imageSet.currentImageSetHasImage(imagePath)) {
        finalFile = file;
    } else {
        finalFile = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/desktoptheme/" + compactName(m_themeName) + "/" + imagePath
            + ".svgz";
        const QString dirPath = QFileInfo(finalFile).absoluteDir().absolutePath();
        KIO::mkdir(QUrl::fromLocalFile(dirPath))->exec();

        KIO::FileCopyJob *job = KIO::file_copy(QUrl::fromLocalFile(file), QUrl::fromLocalFile(finalFile));
        if (!job->exec()) {
            qWarning() << "Error copying" << file << "to" << finalFile;
        }
    }

    auto process = new QProcess(this);
    process->start("bash",
                   {u"-c"_s,
                    u""
                    "cd %2;"
                    "inkscape \"%1.svgz\";"
                    "mv \"%1.svgz\" \"%1.svg.gz\";"
                    "gunzip \"%1.svg.gz\";"
                    "/usr/bin/perl -p -i -e \"s/color:#[^;]*;fill:currentColor/fill:currentColor/g\" \"%1.svg\";"
                    "gzip  \"%1.svg\";"
                    "mv \"%1.svg.gz\" \"%1.svgz\""
                    ""_s.arg(finalFile.mid(finalFile.lastIndexOf("/") + 1).split(".")[0], finalFile.left(finalFile.lastIndexOf("/")))});

    connect(process, &QProcess::finished, this, &ThemeModel::processFinished);
}

void ThemeModel::processFinished()
{
    /*We increment the microversion of the theme: keeps track and will force the cache to be
      discarded in order to reload immediately the graphics*/
    const QString metadataPath(
        QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/desktoptheme/"_L1 % compactName(m_themeName) % "/metadata.json"_L1));

    QFile file(metadataPath);
    (void)file.open(QFile::ReadWrite);
    auto json = QJsonDocument::fromJson(file.readAll()).object();

    auto version = json["KPlugin"_L1]["Version"_L1].toString().split('.');

    if (version.length() < 2) {
        version << QLatin1String("0");
    }
    if (version.length() < 3) {
        version << QLatin1String("0");
    }

    QString newVersion = version.first() + u"."_s + version[1] + u"."_s + QString::number(version.last().toInt() + 1);
    auto kPlugin = json["KPlugin"].toObject();
    kPlugin["Version"] = newVersion;
    json["KPlugin"] = kPlugin;
    file.seek(0);
    file.resize(0);
    file.write(QJsonDocument(json).toJson());
    file.close();
}

void ThemeModel::editThemeMetaData(const QString &name, const QString &author, const QString &email, const QString &license, const QString &website)
{
    const auto metadataPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % "/plasma/desktoptheme/"_L1 % compactName(name)
                            % "/metadata.json"_L1);

    QFile file(metadataPath);
    (void)file.open(QFile::ReadWrite);

    auto json = QJsonDocument::fromJson(file.readAll()).object();

    json[u"X-Plasma-API"_s] = u"5.0"_s;

    auto kPlugin = json[u"KPlugin"_s].toObject();
    auto authors = kPlugin[u"Authors"_s].toArray();
    authors = QJsonArray{QJsonObject{
        {u"Email"_s, email},
        {u"Name"_s, author},
    }};
    kPlugin["Authors"] = authors;
    kPlugin["Name"] = name;
    kPlugin["Category"] = u"Plasma Theme"_s;

    if (!kPlugin.contains("Description")) {
        kPlugin["Description"] = QString();
    }

    kPlugin["EnabledByDefault"] = true;
    if (!kPlugin.contains("Id")) {
        kPlugin[u"Id"_s] = compactName(name);
    }

    kPlugin["Website"] = website;
    kPlugin["License"] = license;

    if (!kPlugin.contains("Version")) {
        kPlugin["Version"] = "1.0";
    }

    json["KPlugin"] = kPlugin;

    file.seek(0);
    file.resize(0);
    file.write(QJsonDocument(json).toJson());
    file.close();
}

void ThemeModel::createNewTheme(const QString &name, const QString &author, const QString &email, const QString &license, const QString &website)
{
    auto dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + u"/plasma/desktoptheme/"_s + compactName(name);
    QDir().mkdir(dir);
    editThemeMetaData(name, author, email, license, website);

    const auto colorsFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "/plasma/desktoptheme/breeze-light/colors");

    const auto finalColorsFile =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/desktoptheme/" + compactName(name) + "/colors";

    KIO::FileCopyJob *job = KIO::file_copy(QUrl::fromLocalFile(colorsFile), QUrl::fromLocalFile(finalColorsFile));
    if (!job->exec()) {
        qWarning() << "Error copying" << colorsFile << "to" << finalColorsFile;
    }

    m_themeListModel->reload();
}

QString ThemeModel::themeFolder()
{
    return QStandardPaths::locate(QStandardPaths::GenericDataLocation, +"plasma/desktoptheme/" + compactName(m_themeName), QStandardPaths::LocateDirectory);
}

bool ThemeModel::isWritable() const
{
    return QFile::exists(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/desktoptheme/" + compactName(m_themeName));
}

#include "moc_thememodel.cpp"
