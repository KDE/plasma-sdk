/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QTableWidgetItem>
#include <QString>

#include "timelineitem.h"

static const int TimeLineItemType = QTableWidgetItem::UserType + 1;

TimeLineItem::TimeLineItem(const QIcon &icon,
        QStringList &dataList,
        const TimeLineItem::ItemIdentifier id,
        const Qt::ItemFlag flag)
        : QTableWidgetItem(0,TimeLineItemType)
{
    setFlags(flag);
    setIcon(icon);
    setText(dataList.takeFirst());
    setToolTip(dataList.takeFirst());
    /*setData(Qt::DecorationRole,QVariant(icon));
    setData(Qt::DisplayRole,QVariant(dataList.takeFirst()));
    setData(Qt::ToolTipRole,QVariant(dataList.takeFirst()));*/
    m_type = id;
    m_sha1hash = dataList.takeFirst();
}

void TimeLineItem::setHash(const QString &hash)
{
    m_sha1hash = hash;
}

void TimeLineItem::setIdentifier(const TimeLineItem::ItemIdentifier id)
{
    m_type = id;
}

QString TimeLineItem::getHash()
{
    return m_sha1hash;
}

TimeLineItem::ItemIdentifier TimeLineItem::getIdentifier()
{
    return m_type;
}

/*QVariant TimeLineItem::data(int role)
{
    switch(role) {
        case Qt::DisplayRole:
            return QVariant(text());
        break;

        case Qt::DecorationRole:
            return QVariant(icon());
        break;

        case Qt::ToolTipRole:
            return QVariant(toolTip());
        break;

        default:
        return QVariant();
    }
}*/
