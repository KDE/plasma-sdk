/******************************************************************************
 * Copyright (C) 2007 by Pino Toscano <pino@kde.org>                          *
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful, but     *
 *    WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *    General Public License for more details.                                *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program; if not, write to the Free Software Foundation  *
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA        *
 *                                                                            *
 ******************************************************************************/

#include "timelineitem.h"

static const int TimeLineItemType = QListWidgetItem::UserType + 1;

TimeLineItem::TimeLineItem(const QIcon &icon,
                           QStringList &dataList,
                           const TimeLineItem::ItemIdentifier id,
                           const Qt::ItemFlag flag)
        : QListWidgetItem(0, TimeLineItemType)

{
    //  Note: we expect dataList is as follows:
    //  dataList(0) = text string that will be showed
    //  dataList(1) = commit string used for tooltip
    //  dataList(2) = sha1 hash

    setFlags(flag);
    setIcon(icon);
    setText(dataList.takeFirst());
    setToolTip(dataList.takeFirst());
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

TimeLineItem::  ItemIdentifier TimeLineItem::getIdentifier()
{
    return m_type;
}
