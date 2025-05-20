/*
 * LnfListModel
 * SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
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

#include "lnflistmodel.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QStandardPaths>

#include <KConfigGroup>
#include <KDesktopFile>

#include <Plasma/Theme>

#include <QDebug>

using namespace Qt::StringLiterals;

LnfListModel::LnfListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    reload();
}

LnfListModel::~LnfListModel() = default;

QHash<int, QByteArray> LnfListModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayRole"},
        {PackageNameRole, "packageNameRole"},
        {PackageDescriptionRole, "packageDescriptionRole"},
        {PackageAuthorRole, "packageAuthorRole"},
    };
}

void LnfListModel::reload()
{
    beginResetModel();
    m_themes.clear();

    // get all desktop themes
    QStringList themes;
    const QStringList &packs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "plasma/look-and-feel", QStandardPaths::LocateDirectory);
    for (const QString &ppath : packs) {
        const QDir cd(ppath);
        const QStringList &entries = cd.entryList(QDir::Dirs | QDir::Hidden);
        for (const QString &pack : entries) {
            const QString _metadata = ppath + u'/' + pack + QStringLiteral("/metadata.json");
            if ((pack != "." && pack != "..") && (QFile::exists(_metadata))) {
                themes << _metadata;
            }
        }
    }

    for (const QString &theme : themes) {
        int themeSepIndex = theme.lastIndexOf('/', -1);
        QString themeRoot = theme.left(themeSepIndex);
        int themeNameSepIndex = themeRoot.lastIndexOf('/', -1);
        QString packageName = themeRoot.right(themeRoot.length() - themeNameSepIndex - 1);

        QFile file(theme);
        if (!file.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open" << theme;
            continue;
        }

        const auto doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            continue;
        }

        const auto obj = doc.object();

        if (obj["KPackageStructure"_L1].toString() != "Plasma/LookAndFeel") {
            continue;
        }

        const auto plugin = obj["KPlugin"].toObject();
        ThemeInfo info;
        info.package = packageName;
        info.themeRoot = themeRoot;

        info.name = plugin["Name"_L1].toString();
        if (info.name.isEmpty()) {
            info.name = packageName;
        }
        const auto authors = plugin["Authors"_L1].toArray();
        for (const auto &author : authors) {
            const auto authorObj = author.toObject();
            info.author = authorObj["Name"].toString();
        }
        info.description = obj["Description"_L1].toString();
        m_themes << info;
    }

    endResetModel();
    emit countChanged();
}

int LnfListModel::rowCount(const QModelIndex &) const
{
    return m_themes.size();
}

QVariant LnfListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const ThemeInfo &themeInfo = m_themes[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return themeInfo.name;
    case PackageNameRole:
        return themeInfo.package;
    case PackageDescriptionRole:
        return themeInfo.description;
    case PackageAuthorRole:
        return themeInfo.author;
    default:
        return {};
    }
}

ThemeInfo LnfListModel::get(int row) const
{
    return m_themes[row];
}

QModelIndex LnfListModel::indexOf(const QString &name) const
{
    QListIterator<ThemeInfo> it(m_themes);
    int i = -1;
    while (it.hasNext()) {
        ++i;
        if (it.next().package == name) {
            return index(i, 0);
        }
    }

    return QModelIndex();
}

#include "moc_lnflistmodel.cpp"
