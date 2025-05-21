/*
 *   SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "lnflogic.h"
#include "lnflistmodel.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUrl>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>

#include <KAboutData>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/PackageLoader>
#include <KSharedConfig>

using namespace Qt::StringLiterals;

LnfLogic::LnfLogic(QObject *parent)
    : QObject(parent)
    , m_themeName(QStringLiteral("org.kde.breeze.desktop"))
    , m_lnfListModel(new LnfListModel(this))
    , m_needsSave(false)
{
    m_package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Plasma/LookAndFeel"));
}

LnfLogic::~LnfLogic()
{
}

void LnfLogic::createNewTheme(const QString &pluginName,
                              const QString &name,
                              const QString &comment,
                              const QString &author,
                              const QString &email,
                              const QString &license,
                              const QString &website)
{
    const QString metadataPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + u"/plasma/look-and-feel/"_s + pluginName);

    QJsonObject obj{
        {"KPackageStructure"_L1, "Plasma/LookAndFeel"_L1},
        {"KPlugin"_L1,
         QJsonObject{
             {"Authors"_L1,
              QJsonArray{QJsonObject{
                  {"Name"_L1, author},
                  {"Email"_L1, email},
              }}},
             {"Website"_L1, website},
             {"Description"_L1, comment},
             {"Id"_L1, pluginName},
             {"Name"_L1, name},
             {"License"_L1, license},
         }},
        {"Keywords"_L1, "Desktop;Workspace;Appearance;Look and Feel;"_L1},
        {"X-Plasma-APIVersion"_L1, "2"_L1},
        {"X-Plasma-MainScript"_L1, "default"_L1},
    };

    QJsonDocument doc(obj);
    QDir dir(metadataPath);
    dir.mkpath(".");

    QFile file(metadataPath + u"/metadata.json"_s);
    if (!file.open(QFile::WriteOnly)) {
        return;
    }

    file.write(doc.toJson());

    dumpPlasmaLayout(pluginName);
    dumpDefaultsConfigFile(pluginName);

    m_lnfListModel->reload();
}

void LnfLogic::dumpPlasmaLayout(const QString &pluginName)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell", "dumpCurrentLayoutJS");
    QDBusPendingCall pcall = QDBusConnection::sessionBus().asyncCall(message);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);

    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, pluginName](QDBusPendingCallWatcher *watcher) {
        const QDBusMessage &msg = watcher->reply();
        watcher->deleteLater();
        if (watcher->isError()) {
            emit messageRequested(ErrorLevel::Error, i18n("Cannot retrieve the current Plasma layout."));
            return;
        }

        const QString layout = msg.arguments().first().toString();
        QDir themeDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % pluginName);
        if (!themeDir.mkpath("contents/layouts")) {
            qWarning() << "Impossible to create the layouts directory in the look and feel package";
            emit messageRequested(ErrorLevel::Error, i18n("Impossible to create the layouts directory in the look and feel package"));
            return;
        }

        QFile layoutFile(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % pluginName
                         % QLatin1String("/contents/layouts/org.kde.plasma.desktop-layout.js"));
        if (layoutFile.open(QIODevice::WriteOnly)) {
            layoutFile.write(layout.toUtf8());
            layoutFile.close();
        } else {
            qWarning() << "Impossible to write to org.kde.plasma.desktop-layout.js";
            emit messageRequested(ErrorLevel::Error, i18n("Impossible to write to org.kde.plasma.desktop-layout.js"));
            return;
        }
        emit messageRequested(ErrorLevel::Info, i18n("Plasma Layout successfully duplicated"));
    });
}

void LnfLogic::dumpDefaultsConfigFile(const QString &pluginName)
{
    // write the defaults file, read from kde config files and save to the defaultsrc
    KConfig defaultsConfig(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % pluginName
                           % "/contents/defaults");

    KConfigGroup defaultsConfigGroup(&defaultsConfig, "kdeglobals");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "KDE");

    // widget style
    KConfigGroup systemCG(KSharedConfig::openConfig(QStringLiteral("kdeglobals")), "KDE");
    defaultsConfigGroup.writeEntry("widgetStyle", systemCG.readEntry("widgetStyle", QStringLiteral("breeze")));

    // color scheme (TODO: create an in-place color scheme?)
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kdeglobals");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "General");
    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("kdeglobals")), "General");
    defaultsConfigGroup.writeEntry("ColorScheme", systemCG.readEntry("ColorScheme", QStringLiteral("Breeze")));

    // icon theme
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kdeglobals");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "Icons");
    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("kdeglobals")), "Icons");
    defaultsConfigGroup.writeEntry("Theme", systemCG.readEntry("Theme", QStringLiteral("breeze")));

    // plasma theme
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "plasmarc");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "Theme");
    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("plasmarc")), "Theme");
    defaultsConfigGroup.writeEntry("name", systemCG.readEntry("name", QStringLiteral("default")));

    // cursor theme
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kcminputrc");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "Mouse");
    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("kcminputrc")), "Mouse");
    defaultsConfigGroup.writeEntry("cursorTheme", systemCG.readEntry("cursorTheme", QStringLiteral("breeze_cursors")));

    // KWin window switcher theme
    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("kwinrc")), "TabBox");
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kwinrc");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "WindowSwitcher");
    defaultsConfigGroup.writeEntry("LayoutName", systemCG.readEntry("LayoutName", QStringLiteral("org.kde.breeze.desktop")));

    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kwinrc");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "DesktopSwitcher");
    defaultsConfigGroup.writeEntry("LayoutName", systemCG.readEntry("DesktopLayout", QStringLiteral("org.kde.breeze.desktop")));

    systemCG = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("kwinrc")), "org.kde.kdecoration2");
    defaultsConfigGroup = KConfigGroup(&defaultsConfig, "kwinrc");
    defaultsConfigGroup = KConfigGroup(&defaultsConfigGroup, "org.kde.kdecoration2");
    defaultsConfigGroup.writeEntry("library", systemCG.readEntry("library", QStringLiteral("org.kde.breeze")));
    defaultsConfigGroup.writeEntry("theme", systemCG.readEntry("theme", QString()));

    emit messageRequested(ErrorLevel::Info, i18n("Defaults config file saved from your current setup"));
}

void LnfLogic::dumpCurrentPlasmaLayout()
{
    dumpPlasmaLayout(m_themeName);
}

void LnfLogic::save()
{
    QJsonObject obj{
        {"KPackageStructure"_L1, "Plasma/LookAndFeel"_L1},
        {"KPlugin"_L1,
         QJsonObject{
             {"Authors"_L1,
              QJsonArray{QJsonObject{
                  {"Name"_L1, author()},
                  {"Email"_L1, email()},
              }}},
             {"Website"_L1, website()},
             {"Description"_L1, comment()},
             {"Id"_L1, m_package.metadata().pluginId()},
             {"Name"_L1, name()},
             {"License"_L1, license()},
         }},
        {"Keywords"_L1, "Desktop;Workspace;Appearance;Look and Feel;"_L1},
        {"X-Plasma-APIVersion"_L1, "2"_L1},
        {"X-Plasma-MainScript"_L1, "default"_L1},
    };

    QJsonDocument doc(obj);

    QFile file(m_package.path() + u"/metadata.json"_s);
    if (!file.open(QFile::WriteOnly)) {
        Q_EMIT messageRequested(ErrorLevel::Error, file.errorString());
        return;
    }

    file.write(doc.toJson());

    m_tempMetadata.clear();
    m_needsSave = false;
    if (m_performLayoutDump) {
        dumpCurrentPlasmaLayout();
        m_performLayoutDump = false;
    }
    if (m_performDefaultsDump) {
        dumpDefaultsConfigFile(m_themeName);
        m_performDefaultsDump = false;
    }

    emit needsSaveChanged();

    // HACK
    m_package.setPath(QString());
    m_package.setPath(m_themeName);
}

LnfListModel *LnfLogic::lnfList()
{
    return m_lnfListModel;
}

QString LnfLogic::themeFolder() const
{
    return m_package.path();
}

bool LnfLogic::isWritable() const
{
    return QFile::exists(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/plasma/look-and-feel/" + m_themeName);
}

QString LnfLogic::theme() const
{
    return m_themeName;
}

void LnfLogic::setTheme(const QString &theme)
{
    if (theme == m_themeName) {
        return;
    }

    qWarning() << "theme" << theme;

    m_tempMetadata.clear();
    m_themeName = theme;
    m_package.setPath(theme);
    m_needsSave = false;
    emit needsSaveChanged();
    emit themeChanged();
    emit thumbnailPathChanged();
    emit nameChanged();
    emit commentChanged();
    emit authorChanged();
    emit emailChanged();
    emit websiteChanged();
    emit licenseChanged();
}

QString LnfLogic::name() const
{
    if (m_tempMetadata.contains(QStringLiteral("Name"))) {
        return m_tempMetadata.value(QStringLiteral("Name"));
    }
    return m_package.metadata().name();
}

void LnfLogic::setName(const QString &name)
{
    if (LnfLogic::name() == name) {
        return;
    }

    m_tempMetadata[QStringLiteral("Name")] = name;
    m_needsSave = true;
    emit needsSaveChanged();
    emit nameChanged();
}

QString LnfLogic::comment() const
{
    if (m_tempMetadata.contains(QStringLiteral("Comment"))) {
        return m_tempMetadata.value(QStringLiteral("Comment"));
    }
    return m_package.metadata().description();
}

void LnfLogic::setComment(const QString &comment)
{
    if (LnfLogic::comment() == comment) {
        return;
    }

    m_tempMetadata[QStringLiteral("Comment")] = comment;
    m_needsSave = true;
    emit needsSaveChanged();
    emit commentChanged();
}

QString LnfLogic::author() const
{
    if (m_tempMetadata.contains(QStringLiteral("X-KDE-PluginInfo-Author"))) {
        return m_tempMetadata.value(QStringLiteral("X-KDE-PluginInfo-Author"));
    }
    if (m_package.metadata().authors().isEmpty()) {
        return QString();
    }
    return m_package.metadata().authors().first().name();
}

void LnfLogic::setAuthor(const QString &author)
{
    if (LnfLogic::author() == author) {
        return;
    }

    m_tempMetadata[QStringLiteral("X-KDE-PluginInfo-Author")] = author;
    m_needsSave = true;
    emit needsSaveChanged();
    emit authorChanged();
}

QString LnfLogic::email() const
{
    if (m_tempMetadata.contains(QStringLiteral("X-KDE-PluginInfo-Email"))) {
        return m_tempMetadata.value(QStringLiteral("X-KDE-PluginInfo-Email"));
    }
    if (m_package.metadata().authors().isEmpty()) {
        return QString();
    }
    return m_package.metadata().authors().first().emailAddress();
}

void LnfLogic::setEmail(const QString &email)
{
    if (LnfLogic::email() == email) {
        return;
    }

    m_tempMetadata[QStringLiteral("X-KDE-PluginInfo-Email")] = email;
    m_needsSave = true;
    emit needsSaveChanged();
    emit emailChanged();
}

QString LnfLogic::website() const
{
    if (m_tempMetadata.contains(QStringLiteral("X-KDE-PluginInfo-Website"))) {
        return m_tempMetadata.value(QStringLiteral("X-KDE-PluginInfo-Website"));
    }
    return m_package.metadata().website();
}

void LnfLogic::setWebsite(const QString &website)
{
    if (LnfLogic::website() == website) {
        return;
    }

    m_tempMetadata[QStringLiteral("X-KDE-PluginInfo-Website")] = website;
    m_needsSave = true;
    emit needsSaveChanged();
    emit websiteChanged();
}

QString LnfLogic::license() const
{
    if (m_tempMetadata.contains(QStringLiteral("X-KDE-PluginInfo-License"))) {
        return m_tempMetadata.value(QStringLiteral("X-KDE-PluginInfo-License"));
    }
    return m_package.metadata().license();
}

void LnfLogic::setLicense(const QString &license)
{
    if (LnfLogic::license() == license) {
        return;
    }

    m_tempMetadata[QStringLiteral("X-KDE-PluginInfo-License")] = license;
    m_needsSave = true;
    emit needsSaveChanged();
    emit licenseChanged();
}

bool LnfLogic::performLayoutDump() const
{
    return m_performLayoutDump;
}

void LnfLogic::setPerformLayoutDump(bool dump)
{
    if (m_performLayoutDump == dump) {
        return;
    }

    m_needsSave = true;
    emit needsSaveChanged();
    m_performLayoutDump = dump;
    emit performLayoutDumpChanged();
}

bool LnfLogic::performDefaultsDump() const
{
    return m_performDefaultsDump;
}

void LnfLogic::setPerformDefaultsDump(bool dump)
{
    if (m_performDefaultsDump == dump) {
        return;
    }

    m_needsSave = true;
    emit needsSaveChanged();
    m_performDefaultsDump = dump;
    emit performDefaultsDumpChanged();
}

bool LnfLogic::needsSave()
{
    return m_needsSave;
}

QString LnfLogic::thumbnailPath() const
{
    // don't fallback
    const QString path = m_package.filePath("previews", QStringLiteral("preview.png"));
    if (path.contains(m_package.path())) {
        return path;
    }

    return QString();
}

void LnfLogic::processThumbnail(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    QDir themeDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % m_themeName);
    if (!themeDir.mkpath("contents/previews")) {
        qWarning() << "Impossible to create the layouts directory in the look and feel package";
    }

    QFile imageFile(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % m_themeName
                    % QLatin1String("/contents/previews/preview.png"));
    if (!imageFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Impossible to write to the thumbnail file";
        return;
    }

    QImage image(QUrl(path).path());
    if (image.isNull()) {
        qWarning() << "invalid image";
        return;
    }
    image = image.scaledToWidth(512, Qt::SmoothTransformation);

    image.save(&imageFile, "PNG"); // writes image into ba in PNG format
    imageFile.close();

    // copy the fullscreen preview
    QFile fullScreenImageFile(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) % QLatin1String("/plasma/look-and-feel/") % m_themeName
                              % QLatin1String("/contents/previews/fullscreenpreview.jpg"));
    if (!fullScreenImageFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Impossible to write to the thumbnail file";
        return;
    }

    QImage fullScreenImage(QUrl(path).path());
    if (fullScreenImage.isNull()) {
        qWarning() << "invalid image";
        return;
    }

    fullScreenImage.save(&fullScreenImageFile, "JPG"); // writes image into ba in PNG format
    fullScreenImageFile.close();

    Q_EMIT thumbnailPathChanged();
}

QString LnfLogic::openFile()
{
    return QFileDialog::getOpenFileName(nullptr,
                                        i18n("Open Image"),
                                        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                        i18n("Image Files (*.png *.jpg *.bmp)"));
}

#include "moc_lnflogic.cpp"
