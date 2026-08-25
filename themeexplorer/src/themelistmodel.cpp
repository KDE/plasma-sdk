/*
 * ThemeListModel
 * SPDX-FileCopyrightText: 2002 Karol Szwed <gallium@kde.org>
 * SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>
 * SPDX-FileCopyrightText: 2007 Urs Wolfer <uwolfer @ kde.org>
 * SPDX-FileCopyrightText: 2009 Davide Bettio <davide.bettio@kdemail.net>
 * SPDX-FileCopyrightText: 2007 Paolo Capriotti <p.capriotti@gmail.com>
 * SPDX-FileCopyrightText: 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 * SPDX-FileCopyrightText: 2008 Petri Damsten <damu@iki.fi>
 * SPDX-FileCopyrightText: 2000 TrollTech AS.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "themelistmodel.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QStandardPaths>

#include <KDesktopFile>

#include <Plasma/Theme>

#include <QDebug>

ThemeListModel::ThemeListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    reload();
}

QHash<int, QByteArray> ThemeListModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {PackageNameRole, "packageNameRole"},
        {PackageDescriptionRole, "packageDescriptionRole"},
        {PackageAuthorRole, "packageAuthorRole"},
        {PackageVersionRole, "packageVersionRole"},
    };
}

void ThemeListModel::reload()
{
    beginResetModel();
    m_themes.clear();

    QStringList themes;
    const QStringList &packs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "plasma/desktoptheme", QStandardPaths::LocateDirectory);
    for (const QString &ppath : packs) {
        const QDir cd(ppath);
        const QStringList &entries = cd.entryList(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
        for (const QString &pack : entries) {
            const QString metadata = ppath + QLatin1Char('/') + pack + QStringLiteral("/metadata.json");
            if (QFile::exists(metadata)) {
                themes << metadata;
            }
        }
    }

    for (const QString &theme : themes) {
        const auto themeSepIndex = theme.lastIndexOf('/', -1);
        const auto themeRoot = theme.left(themeSepIndex);
        const auto themeNameSepIndex = themeRoot.lastIndexOf('/', -1);
        const auto packageName = themeRoot.right(themeRoot.length() - themeNameSepIndex - 1);

        QFile file(theme);
        (void)file.open(QFile::ReadOnly);
        const auto json = QJsonDocument::fromJson(file.readAll()).object();

        auto name = json["KPlugin"]["Name"].toString();
        if (name.isEmpty()) {
            name = packageName;
        }
        const auto comment = json["KPlugin"]["Description"].toString();

        auto authors = json["KPlugin"]["Author"].toArray();
        const auto author = authors.size() > 0 ? authors[0].toObject()["Name"].toString() : QString();
        const auto version = json["KPlugin"]["Version"].toString();

        ThemeInfo info;
        info.package = packageName;
        info.description = comment;
        info.author = author;
        info.version = version;
        info.themeRoot = themeRoot;
        m_themes[name] = info;
    }

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
    std::advance(it, index.row());

    switch (role) {
    case Qt::DisplayRole:
        return it.key();
    case PackageNameRole:
        return it->package;
    case PackageDescriptionRole:
        return it->description;
    case PackageAuthorRole:
        return it->author;
    case PackageVersionRole:
        return it->version;
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
