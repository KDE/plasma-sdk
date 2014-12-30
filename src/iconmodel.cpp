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
#include <KSharedConfig>

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
    m_roleNames.insert(Size, "size");


    KConfigGroup cg(KSharedConfig::openConfig("cuttlefishrc"), "CuttleFish");
    const QString themeName = cg.readEntry("theme", "default");

    Plasma::Theme theme;
    qDebug() << "Setting theme, package " << themeName;
    theme.setUseGlobalSettings(false);
    theme.setThemeName(themeName); // needs to happen after setUseGlobalSettings, since that clears themeName
    m_themes << "default"
             << "hicolor"
             << "locolor"
             << "oxygen"
             << "Tango"
             << "gnome";

    load();
    qDebug() << m_roleNames;
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
        qDebug() << "Requesting " << key(role) << m_data[icon][key(role)];
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
    beginResetModel();
    endResetModel();
    //emit QAbstractItemModel::modelReset();
//     auto topleft = index(0);
//     auto bottomright = index(rowCount(topleft));
//     emit dataChanged(topleft, bottomright);
}


void IconModel::add(const QFileInfo &info)
{
    QVariantMap data;
    const QString fname = info.fileName();

    QString icon = fname;
    if (fname.endsWith(".png")) {

        icon.remove(".png");

    } else if (fname.endsWith(".svgz")) {
        icon.remove(".svgz");
    }
    //qDebug() << "found Icon: " << info.fileName() << icon;
    if (!m_icons.contains(icon)) {

        data["fullPath"] = info.absoluteFilePath();
        data["iconName"] = icon;
        data["fileName"] = info.fileName();

        m_data.insert(icon, data);
        m_icons << icon;
        //update();
    }
}

void IconModel::remove(const QString& url)
{
    qDebug() << "IconModel::remove() TODO";

}

QString IconModel::category() const
{
    return m_category;
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
    return QString();
}

void IconModel::setFilter(const QString &filter)
{
    qDebug() << "Filter: " << filter;
}

void IconModel::load()
{
    qDebug() << "Loading from " << m_theme;
    const QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    const QStringList nameFilters = QStringList();

    QDirIterator it(QStringLiteral("/usr/share/icons/") + m_theme , nameFilters, QDir::Files, flags);

    while (it.hasNext()) {
        it.next();
        const QFileInfo &info = it.fileInfo();
//         qDebug() << "..." << info.absoluteFilePath();
        add(info);
    }

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
        beginResetModel();
        m_data.clear();
        m_icons.clear();
        m_theme = theme;
        load();
        endResetModel();
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
        emit plasmaThemeChanged();
    }
}

