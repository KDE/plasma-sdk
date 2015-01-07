/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

#include "iconmodel.h"

#include <QDebug>
#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QIcon>
#include <QStandardPaths>
#include <QJsonDocument>

#include <KConfigGroup>
#include <KIconLoader>
#include <KIconTheme>
#include <KSharedConfig>
#include <PackageLoader>

#include <Plasma/Theme>

using namespace CuttleFish;

IconModel::IconModel(QObject *parent) :
    QAbstractListModel(parent),
    m_theme(QStringLiteral("breeze"))
{
    m_roleNames.insert(FileName, "fileName");
    m_roleNames.insert(IconName, "iconName");
    m_roleNames.insert(Icon, "icon");
    m_roleNames.insert(FullPath, "fullPath");
    m_roleNames.insert(Category, "category");
    m_roleNames.insert(Scalable, "scalable");
    m_roleNames.insert(Sizes, "sizes");
    m_roleNames.insert(Theme, "iconTheme");
    m_roleNames.insert(Type, "type");


    KConfigGroup cg(KSharedConfig::openConfig("cuttlefishrc"), "CuttleFish");
    const QString themeName = cg.readEntry("theme", "default");

    Plasma::Theme theme;
    qDebug() << "Setting Plasma theme" << themeName;
    theme.setUseGlobalSettings(false);
    theme.setThemeName(themeName); // needs to happen after setUseGlobalSettings, since that clears themeName

    QList<KPluginMetaData> themepackages = KPackage::PackageLoader::self()->listPackages(QString(), "plasma/desktoptheme");
    foreach (const KPluginMetaData &pkg, themepackages) {
        m_plasmathemes << pkg.pluginId();
    }

    m_svgIcons[QStringLiteral("amarok")] = QStringList() << "amarok";
    m_svgIcons[QStringLiteral("audio")] = QStringList() << "audio-volume-muted"
                                                        << "audio-volume-low"
                                                        << "audio-volume-medium"
                                                        << "audio-volume-high" ;

    m_svgIcons[QStringLiteral("battery")] = QStringList() << "Battery"
                                                        << "Acadapter"
                                                        << "Unavailable"
                                                        << "Fill10"
                                                        << "Fill20"
                                                        << "Fill30"
                                                        << "Fill40"
                                                        << "Fill50"
                                                        << "Fill60"
                                                        << "Fill70"
                                                        << "Fill80"
                                                        << "Fill90"
                                                        << "Fill100";
    m_svgIcons[QStringLiteral("device")] = QStringList() << "device-notfier";
    m_svgIcons[QStringLiteral("juk")] = QStringList() << "juk";
    m_svgIcons[QStringLiteral("kget")] = QStringList() << "kget";
    m_svgIcons[QStringLiteral("klippper")] = QStringList() << "klippper";
    m_svgIcons[QStringLiteral("konversation")] = QStringList() << "konversation";
    m_svgIcons[QStringLiteral("kopete")] = QStringList() << "konversation"
                                                         << "kopete-offline";
    m_svgIcons[QStringLiteral("korgac")] = QStringList() << "korgac";
    m_svgIcons[QStringLiteral("ktorrent")] = QStringList() << "ktorrent";
    m_svgIcons[QStringLiteral("kget")] = QStringList() << "kget";

    m_svgIcons[QStringLiteral("battery")] = QStringList() << "Battery"
                                                        << "Acadapter"
                                                        << "Unavailable"
                                                        << "Fill10"
                                                        << "Fill20"
                                                        << "Fill30"
                                                        << "Fill40"
                                                        << "Fill50"
                                                        << "Fill60"
                                                        << "Fill70"
                                                        << "Fill80"
                                                        << "Fill90"
                                                        << "Fill100";
    m_svgIcons[QStringLiteral("message-indicator")] = QStringList() << "normal"
                                                         << "new";

    m_svgIcons[QStringLiteral("nepomuk")] = QStringList() << "nepomuk";
    m_svgIcons[QStringLiteral("network")] = QStringList() << "network-wired-activated"
                                                        << "network-wired"
                                                        << "network-wireless-available"
                                                        << "network-wireless-disconnected"
                                                        << "flightmode-off"
                                                        << "flightmode-on"
                                                        << "network-wireless-0"
                                                        << "network-wireless-20"
                                                        << "network-wireless-25"
                                                        << "network-wireless-40"
                                                        << "network-wireless-50"
                                                        << "network-wireless-60"
                                                        << "network-wireless-75"
                                                        << "network-wireless-80"
                                                        << "network-wireless-100"
                                                        << "network-mobile-20"
                                                        << "network-mobile-40"
                                                        << "network-mobile-60"
                                                        << "network-mobile-80"
                                                        << "network-mobile-100";
    m_svgIcons[QStringLiteral("preferences")] = QStringList() << "preferences-system-bluetooth"
                                                        << "preferences-system-bluetooth-inactive"
                                                        << "preferences-desktop-texttospeach"
                                                        << "preferences-desktop-display-randr"
                                                        << "preferences-activities";
    m_svgIcons[QStringLiteral("printer")] = QStringList() << "printer";
    m_svgIcons[QStringLiteral("quassel")] = QStringList() << "quassel"
                                                        << "quassel_inactive"
                                                        << "quassel_message";
    m_svgIcons[QStringLiteral("wallet")] = QStringList() << "wallet"
                                                        << "wallet-open"
                                                        << "wallet-closed";

    load();
    //qDebug() << m_roleNames;
}


QHash<int, QByteArray> IconModel::roleNames() const
{
    return m_roleNames;
}

int IconModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_data.size() <= 0) {
        return 0;
    } else {
        return m_data.size();
    }
}

QVariant IconModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        QString icon = m_icons.at(index.row());
        switch (role) {
        case IconName:
            return icon;
        }

        //qDebug() << "Requesting " << key(role) << m_data[icon][key(role)];
        return m_data[icon][key(role)];
    }
    return QVariant();
}

QString IconModel::key(int role) const
{
    return QString::fromLocal8Bit(m_roleNames[role]);
}


void IconModel::update()
{
    // FIXME: Can we be more fine-grained, please?
//     beginResetModel();
//     endResetModel();
    //emit QAbstractItemModel::modelReset();
//     auto topleft = index(0);
//     auto bottomright = index(rowCount(topleft));
//     emit dataChanged(topleft, bottomright);
}


void IconModel::add(const QFileInfo &info, const QString &cat)
{
    if (!m_categories.contains(cat)) {
        m_categories << cat;
        emit categoriesChanged();
    }

    const QString fname = info.fileName();
    bool scalable = false;
    QString icon = fname;
    if (fname.endsWith(".png")) {
        icon.remove(".png");
    } else if (fname.endsWith(".svgz")) {
        icon.remove(".svgz");
        scalable = true;
    }
    QVariantMap &data = m_data[icon];
    //qDebug() << "found Icon: " << info.fileName() << icon;
    if (!m_icons.contains(icon)) {

        data["fullPath"] = info.absoluteFilePath();
        data["iconName"] = icon;
        data["fileName"] = info.fileName();
        data["category"] = cat;
        data["type"] = QStringLiteral("icon");
        data["scalable"] = scalable;
        data["iconTheme"] = "breeze";

        m_icons << icon;
    }
    if (scalable && !data["scalable"].toBool()) {
        data["scalable"] = true;
    }

    QStringList _s = info.path().split('/');
    if (_s.count() > 2) {
        QString size = _s[_s.count()-2]; // last but one is size, last is category
        if (size.indexOf("x") > 1) {
            size = size.split("x")[0];
            QStringList sizes = data["sizes"].toStringList();
            if (!sizes.contains(size)) {
                //qDebug() << "Size added" <<  sizes << size << data["iconName"];
                sizes << size;
                data["sizes"] = sizes;
            }
        }
    }

    //qDebug() << "Size: " << size;
}

void IconModel::addSvgIcon(const QString &file, const QString &icon)
{

}

void IconModel::remove(const QString& url)
{
    qDebug() << "IconModel::remove() TODO";

}

QString IconModel::category() const
{
    return m_category;
}

QStringList IconModel::categories() const
{
    return m_categories;
}


void IconModel::setCategory(const QString& cat)
{
    if (cat != m_category) {
        m_category = cat;
        emit categoryChanged();
    }
}

QString IconModel::filter() const
{
    return m_filter;
}

void IconModel::setFilter(const QString &filter)
{
    //qDebug() << "Filter: " << filter;
    if (m_filter != filter) {
        m_filter = filter;
        load();
        emit filterChanged();
        emit svgIconsChanged();
    }
}

void IconModel::load()
{
    qDebug() << "Loading from " << m_theme;
    const QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    const QStringList nameFilters = QStringList();

    // FIXME: use QStandardPaths

    beginResetModel();
    m_data.clear();
    m_icons.clear();
    m_categories.clear();

    const QString iconTheme = KIconLoader::global()->theme()->internalName();

    qDebug() << "IconTHeme: " << KIconLoader::global()->theme()->internalName();
    qDebug() << "iconPath: " << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "icons/"+iconTheme, QStandardPaths::LocateDirectory);

    QStringList searchPaths;


    QStringList iconThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "icons/"+iconTheme, QStandardPaths::LocateDirectory);

    if (iconThemePaths.count() > 0) {
        searchPaths << iconThemePaths;
    }

    QStringList hicolorThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "icons/hicolor", QStandardPaths::LocateDirectory);
    if (hicolorThemePaths.count() > 0) {
        searchPaths << hicolorThemePaths;
    }

    foreach (const QString &iconPath, searchPaths) {

        QDirIterator cats(iconPath, nameFilters, QDir::Dirs, QDirIterator::NoIteratorFlags);
        while (cats.hasNext()) {
            cats.next();
            const QString fpath = cats.filePath();
//             qDebug() << "Category: !!! ..." << cats.path() << cats.filePath() << fpath;
            const QString category = cats.fileName();
            if (category != "." && category != "..") {
                QDirIterator it(fpath, nameFilters, QDir::Files, flags);
                //qDebug() << "Parsing files." << fpath;
                while (it.hasNext()) {
                    it.next();
                    const QFileInfo &info = it.fileInfo();
                    if (match(info)) {
                        //qDebug() << "..." << info.absoluteFilePath();
                        add(info, categoryFromPath(info.absoluteFilePath()));
                    }
                }
            }
        }
    }

    svgIcons();
    endResetModel();

}

bool IconModel::match(const QFileInfo& info)
{
    bool ok = false;

    // name filter
    if (m_filter.isEmpty() || info.fileName().indexOf(m_filter) != -1) {
        ok = true;
    }
    return ok;
}


QStringList IconModel::themes() const
{
    return m_themes;
}

QString IconModel::theme() const
{
    return m_theme;
}

void IconModel::setTheme(const QString& theme)
{
    if (theme != m_theme) {
        qDebug() << "Theme is now: " << theme;
        m_theme = theme;
        load();
        emit themeChanged();
    }
}

QString IconModel::plasmaTheme() const
{
    return m_plasmatheme;
}

void IconModel::setPlasmaTheme(const QString& ptheme)
{
    if (m_plasmatheme != ptheme) {
        m_plasmatheme = ptheme;
        Plasma::Theme theme;
        qDebug() << "Setting theme, package " << ptheme;
        theme.setThemeName(ptheme);
        load();
        emit plasmaThemeChanged();
    }
}

QStringList IconModel::plasmathemes() const
{
    return m_plasmathemes;
}

void IconModel::svgIcons()
{
    QVariantList out;

    foreach (const QString &file, m_svgIcons.keys()) {
        foreach (const QString &icon, m_svgIcons[file].toStringList()) {
            if (m_filter.isEmpty() ||
               (file.indexOf(m_filter) != -1 || icon.indexOf(m_filter) != -1)) {

                QVariantMap &data = m_data[icon];
                //qDebug() << "found Icon: " << info.fileName() << icon;
                if (!m_icons.contains(icon)) {

                    data["fullPath"] = "";
                    data["iconName"] = icon;
                    data["fileName"] = file;
                    data["category"] = "system";
                    data["type"] = QStringLiteral("svg");
                    data["scalable"] = true;
                    data["iconTheme"] = m_plasmatheme;

                    m_icons << icon;
                }
                QVariantMap vm;
                vm["fileName"] = file;
                vm["iconName"] = icon;
    //             qDebug() << "Adding " << file << icon;
                out << vm;


            }
        }
    }

    //return out;
}

QString IconModel::categoryFromPath(const QString& path)
{
    QStringList cats;
    cats << "actions" << "apps" << "places" << "status";
    const QStringList _p1 = path.split("/icons/");
    if (_p1.count() > 1) {
        foreach (const QString &cat, cats) {
            if (_p1[1].indexOf("cat") != -1) {
                return cat;
            }
        }
    }
    return QString();
}

