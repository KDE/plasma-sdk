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
//#include <QIcon>
#include <QStandardPaths>

#include <QJsonDocument>


using namespace CuttleFish;

IconModel::IconModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_roleNames.insert(FileName, "fileName");
    m_roleNames.insert(IconName, "iconName");
    m_roleNames.insert(Icon, "icon");
    m_roleNames.insert(FullPath, "fullPath");
    m_roleNames.insert(Category, "category");
    m_roleNames.insert(Size, "size");

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

//        QJsonObject currentData = m_data.at(index.row()).toObject();
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
        update();
    }
}

void IconModel::remove(const QString& url)
{


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
    const QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    const QStringList nameFilters = QStringList();

    QDirIterator it(QStringLiteral("/usr/share/icons/breeze/"), nameFilters, QDir::Files, flags);
    while (it.hasNext()) {
        it.next();
        const QFileInfo &info = it.fileInfo();
        add(info);
    }
}

