/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
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
#include <QElapsedTimer>
#include <QFile>
#include <QIcon>
#include <QStandardPaths>
#include <QJsonDocument>

#include <KConfigGroup>
#include <KColorSchemeManager>
#include <KIconLoader>
#include <KIconTheme>
#include <KSharedConfig>
#include <KPackage/PackageLoader>

#include <algorithm>
#include <cstring>
#include <iostream>

using namespace CuttleFish;

static QTextStream cout(stdout);

IconModel::IconModel(QObject *parent) :
    QAbstractListModel(parent)
    , m_loading(false)
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

    connect(this, &IconModel::categoryChanged, this, &IconModel::load);

    m_categories = QStringList() << "all" \
        << "actions"
        << "animations"
        << "apps"
        << "categories"
        << "devices"
        << "emblems"
        << "emotes"
        << "filesystems"
        << "international"
        << "mimetypes"
        << "places"
        << "status";

    load();
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
        return m_data[icon][key(role)];
    }
    return QVariant();
}

QString IconModel::key(int role) const
{
    return QString::fromLocal8Bit(m_roleNames[role]);
}

void IconModel::add(const QFileInfo &info, const QString &cat)
{
    QStringList cats;
    Q_FOREACH (auto c, m_categories) {
        cats << c.toLower();
    }

    if (!cats.contains(cat)) {
        m_categories << cat;
        emit categoriesChanged();
    }

    const QString fname = info.fileName();
    bool scalable = false;
    QString icon = fname;
    if (fname.endsWith(QLatin1String(".png"))) {
        icon.remove(".png");
    } else if (fname.endsWith(QLatin1String(".svgz"))) {
        icon.remove(".svgz");
        scalable = true;
    } else if (fname.endsWith(QLatin1String(".svg"))) {
        icon.remove(".svg");
        scalable = true;
    }
    QVariantMap &data = m_data[icon];
    if (!m_icons.contains(icon)) {

        data["fullPath"] = info.absoluteFilePath();
        data["iconName"] = icon;
        data["fileName"] = info.fileName();
        data["category"] = cat;
        data["type"] = QStringLiteral("icon");
        data["scalable"] = scalable;
        data["iconTheme"] = QStringLiteral("breeze");

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
    }
}

void IconModel::load()
{
    //qDebug() << "\n -- Loading (category / filter) : " << m_category << m_filter;
    m_loading = true;
    emit loadingChanged();

    QElapsedTimer tt;
    tt.start();
    const QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    const QStringList nameFilters = QStringList();

    beginResetModel();
    m_data.clear();
    m_icons.clear();
    //sm_categories.clear();
    QString iconTheme;
    if (KIconLoader::global()) {
        iconTheme = KIconLoader::global()->theme()->internalName();
    } else {
        return;
    }
    QStringList searchPaths;

    QStringList iconThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("icons/") + iconTheme, QStandardPaths::LocateDirectory);

    if (iconThemePaths.count() > 0) {
        searchPaths << iconThemePaths;
    }

    QStringList hicolorThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("icons/hicolor"), QStandardPaths::LocateDirectory);
    if (hicolorThemePaths.count() > 0) {
        searchPaths << hicolorThemePaths;
    }

    foreach (const QString &iconPath, searchPaths) {
        QDirIterator cats(iconPath, nameFilters, QDir::Dirs, QDirIterator::NoIteratorFlags);
        while (cats.hasNext()) {
            cats.next();
            const QString fpath = cats.filePath();
            const QString category = cats.fileName();
            if (category != "." && category != "..") {
                QDirIterator it(fpath, nameFilters, QDir::Files, flags);
                while (it.hasNext()) {
                    it.next();
                    const QFileInfo &info = it.fileInfo();
                    if (matchIcons(info)) {
                        add(info, categoryFromPath(info.absoluteFilePath()));
                    }
                }
            }
        }
    }

    sort();

    endResetModel();

    m_loading = false;
    emit loadingChanged();
}

void IconModel::sort()
{
    std::sort(m_icons.begin(), m_icons.end());
}

bool IconModel::matchIcons(const QFileInfo& info)
{
    bool ok = false;

    // Category is empty or all? Skip further matching.
    bool catmatch = m_category.isEmpty() || m_category == QStringLiteral("all");
    // category match?
    if (!catmatch && m_category == categoryFromPath(info.absoluteFilePath())) {
        catmatch = true;
    }

    // name filter
    if (m_filter.isEmpty() || info.fileName().indexOf(m_filter) != -1) {
        if (catmatch) {
            ok = true;
        }
    }
    return ok;
}

QString IconModel::categoryFromPath(const QString& path)
{
    QStringList cats;
    Q_FOREACH (auto c, m_categories) {
        cats << c.toLower();
    }
    //cats << "actions" << "apps" << "places" << "status";
    const QStringList _p1 = path.split("/icons/");
    if (_p1.count() > 1) {
        foreach (const QString &cat, cats) {
            if (_p1[1].indexOf(cat) != -1) {
                return cat;
            }
        }
    }
    return QString();
}

bool IconModel::loading()
{
    return m_loading;
}

void IconModel::output(const QString& text)
{
    cout << text.toLocal8Bit();
    cout.flush();
}

QVariantList IconModel::inOtherThemes(const QString& name, int iconSize)
{
    QVariantList list;
    const QStringList themes = KIconTheme::list();
    for (const auto& themeName : themes) {
        const KIconTheme theme(themeName);
        const QString iconPath = theme.iconPathByName(name, iconSize, KIconLoader::MatchBest);
        list.append(QVariantMap({{"themeName", themeName}, {"iconPath", iconPath}}));
    }
    return list;
}
