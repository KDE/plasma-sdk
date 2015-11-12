/*
 * ThemeListModel
 * Copyright (C) 2002 Karol Szwed <gallium@kde.org>
 * Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>
 * Copyright (C) 2007 Urs Wolfer <uwolfer @ kde.org>
 * Copyright (C) 2009 by Davide Bettio <davide.bettio@kdemail.net>

 * Portions Copyright (C) 2007 Paolo Capriotti <p.capriotti@gmail.com>
 * Portions Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 * Portions Copyright (C) 2008 by Petri Damsten <damu@iki.fi>
 * Portions Copyright (C) 2000 TrollTech AS.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "themelistmodel.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QStandardPaths>

#include <KDesktopFile>

#include <Plasma/Theme>
#include <qstandardpaths.h>

#include <QDebug>

ThemeListModel::ThemeListModel( QObject *parent )
: QAbstractListModel( parent )
{
    m_roleNames.insert(PackageNameRole, "packageNameRole");
    m_roleNames.insert(PackageDescriptionRole, "packageDescriptionRole");
    m_roleNames.insert(PackageAuthorRole, "packageAuthorRole");
    m_roleNames.insert(PackageVersionRole, "packageVersionRole");

    reload();
}

ThemeListModel::~ThemeListModel()
{
    clearThemeList();
}

QHash<int, QByteArray> ThemeListModel::roleNames() const
{
    return m_roleNames;
}

void ThemeListModel::clearThemeList()
{
    m_themes.clear();
}

void ThemeListModel::reload()
{
    reset();
    clearThemeList();

    // get all desktop themes
    QStringList themes;
    const QStringList &packs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "plasma/desktoptheme", QStandardPaths::LocateDirectory);
    foreach (const QString &ppath, packs) {
        const QDir cd(ppath);
        const QStringList &entries = cd.entryList(QDir::Dirs | QDir::Hidden);
        foreach (const QString pack, entries) {
            const QString _metadata = ppath+QLatin1Char('/')+pack+QStringLiteral("/metadata.desktop");
            if ((pack != "." && pack != "..") &&
                (QFile::exists(_metadata))) {
                themes << _metadata;
            }
        }
    }

    foreach (const QString &theme, themes) {
        int themeSepIndex = theme.lastIndexOf('/', -1);
        QString themeRoot = theme.left(themeSepIndex);
        int themeNameSepIndex = themeRoot.lastIndexOf('/', -1);
        QString packageName = themeRoot.right(themeRoot.length() - themeNameSepIndex - 1);

        KDesktopFile df(theme);

        if (df.noDisplay()) {
            continue;
        }

        QString name = df.readName();
        if (name.isEmpty()) {
            name = packageName;
        }
        const QString comment = df.readComment();
        const QString author = df.desktopGroup().readEntry("X-KDE-PluginInfo-Author",QString());
        const QString version = df.desktopGroup().readEntry("X-KDE-PluginInfo-Version",QString());


        ThemeInfo info;
        info.package = packageName;
        info.description = comment;
        info.author = author;
        info.version = version;
        info.themeRoot = themeRoot;
        m_themes[name] = info;
    }

    beginResetModel();
    endResetModel();
    emit countChanged();
}

int ThemeListModel::rowCount(const QModelIndex &) const
{
    return m_themes.size();
}

QVariant ThemeListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_themes.size()) {
        return QVariant();
    }

    QMap<QString, ThemeInfo>::const_iterator it = m_themes.constBegin();
    for (int i = 0; i < index.row(); ++i) {
        ++it;
    }

    switch (role) {
        case Qt::DisplayRole:
            return it.key();
        case PackageNameRole:
            return (*it).package;
        case PackageDescriptionRole:
            return (*it).description;
        case PackageAuthorRole:
            return (*it).author;
        case PackageVersionRole:
            return (*it).version;
        default:
            return QVariant();
    }
}

QVariantMap ThemeListModel::get(int row) const
{
    QVariantMap item;

    QModelIndex idx = index(row, 0);

    item["display"] = data(idx, Qt::DisplayRole);
    item["packageNameRole"] = data(idx, PackageNameRole);
    item["packageDescriptionRole"] = data(idx, PackageDescriptionRole);
    item["packageAuthorRole"] = data(idx, PackageAuthorRole);
    item["packageVersionRole"] = data(idx, PackageVersionRole);

    return item;
}

QModelIndex ThemeListModel::indexOf(const QString &name) const
{
    QMapIterator<QString, ThemeInfo> it(m_themes);
    int i = -1;
    while (it.hasNext()) {
        ++i;
        if (it.next().value().package == name) {
            return index(i, 0);
        }
    }

    return QModelIndex();
}

#include "moc_themelistmodel.cpp"
