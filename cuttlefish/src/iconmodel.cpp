/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "iconmodel.h"

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QFile>
#include <QIcon>
#include <QJsonDocument>
#include <QStandardPaths>

#include <KConfigGroup>
#include <KIO/OpenFileManagerWindowJob>
#include <KIconLoader>
#include <KIconTheme>
#include <KSharedConfig>

#include <algorithm>
#include <cstring>
#include <iostream>

using namespace CuttleFish;

static QTextStream cout(stdout);

IconModel::IconModel(QObject *parent)
    : QAbstractListModel(parent)
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

    m_categories = {
        QLatin1String("all"),
        QLatin1String("actions"),
        QLatin1String("animations"),
        QLatin1String("apps"),
        QLatin1String("categories"),
        QLatin1String("devices"),
        QLatin1String("emblems"),
        QLatin1String("emotes"),
        QLatin1String("filesystems"),
        QLatin1String("international"),
        QLatin1String("mimetypes"),
        QLatin1String("places"),
        QLatin1String("status"),
    };

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
    const auto category = cat.toLower();
    if (category.isEmpty()) {
        return;
    }
    if (!m_categories.contains(category)) {
        m_categories.append(category);
        Q_EMIT categoriesChanged();
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

    QStringList split = info.path().split('/');
    if (split.count() > 2) {
        QString size = split[split.count() - 2]; // last but one is size, last is category
        if (size.indexOf("x") > 1) {
            size = size.split("x")[0];
            QStringList sizes = data["sizes"].toStringList();
            if (!sizes.contains(size)) {
                // qDebug() << "Size added" <<  sizes << size << data["iconName"];
                sizes << size;
                data["sizes"] = sizes;
            }
        }
    }
}

QStringList IconModel::categories() const
{
    return m_categories;
}

void IconModel::load()
{
    // qDebug() << "\n -- Loading (category / filter) : " << m_category << m_filter;
    m_loading = true;
    Q_EMIT loadingChanged();

    const QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    const QStringList nameFilters = QStringList();

    beginResetModel();
    m_data.clear();
    m_icons.clear();
    QString iconTheme;
    if (KIconLoader::global()) {
        iconTheme = KIconLoader::global()->theme()->internalName();
    } else {
        return;
    }
    QStringList searchPaths;

    QStringList iconThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, //
                                                           QStringLiteral("icons/") + iconTheme,
                                                           QStandardPaths::LocateDirectory);

    if (iconThemePaths.count() > 0) {
        searchPaths << iconThemePaths;
    }

    QStringList hicolorThemePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, //
                                                              QStringLiteral("icons/hicolor"),
                                                              QStandardPaths::LocateDirectory);
    if (hicolorThemePaths.count() > 0) {
        searchPaths << hicolorThemePaths;
    }

    for (const QString &iconPath : std::as_const(searchPaths)) {
        QDirIterator cats(iconPath, nameFilters, QDir::Dirs, QDirIterator::NoIteratorFlags);
        while (cats.hasNext()) {
            cats.next();
            const QString fpath = cats.filePath();
            const QString category = cats.fileName();
            if (category != QLatin1Char('.') && category != QLatin1String("..")) {
                QDirIterator it(fpath, nameFilters, QDir::Files, flags);
                while (it.hasNext()) {
                    it.next();
                    const QFileInfo &info = it.fileInfo();
                    if (const auto category = categoryFromPath(info.absoluteFilePath()); !category.isEmpty()) {
                        add(info, category);
                    }
                }
            }
        }
    }

    endResetModel();

    m_loading = false;
    Q_EMIT loadingChanged();
}

QString IconModel::categoryFromPath(const QString &path)
{
    const QStringList split = path.split("/icons/");
    if (split.count() > 1) {
        const auto subpath = split[1].toLower();
        for (const QString &category : std::as_const(m_categories)) {
            if (subpath.contains(category)) {
                return category;
            }
        }
    }
    return QString();
}

bool IconModel::loading()
{
    return m_loading;
}

void IconModel::output(const QString &text)
{
    cout << text.toLocal8Bit();
    cout.flush();
}

void IconModel::openContainingFolder(const QString &filename)
{
    KIO::highlightInFileManager({QUrl(filename)});
}

QVariantList IconModel::inOtherThemes(const QString &name, int iconSize)
{
    QVariantList list;
    const QStringList themes = KIconTheme::list();
    for (const auto &themeName : themes) {
        const KIconTheme theme(themeName);
        const QString iconPath = theme.iconPathByName(name, iconSize, KIconLoader::MatchBest);
        list.append(QVariantMap({{"themeName", themeName}, {"iconPath", iconPath}}));
    }
    return list;
}

#include "moc_iconmodel.cpp"
