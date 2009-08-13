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

#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QListWidgetItem>

/**
  * This class represents the generic TimeLine item that will be stored and presented
  * to the user.
  * A TimeLineItem is an object that displays an icon and some text underneath it, with
  * a popup string that describes the content it points to.
  * That content is referenced by a string that represents its sha1 hash, and a
  * TimeLineItem::ItemIdentifier field which specifies the type.
  */
class TimeLineItem : public QListWidgetItem
{
public:
    /**
      * Identifier for TimeLineItem object.
      */
    enum ItemIdentifier {
        OutsideWorkingDir = 0, /**< The TimeLineItem represents an object that it isn't inside the working directory. */
        NotACommit = 1, /**< The TimeLineItem represents an object that is not yet a commit. */
        Commit = 2, /**< The TimeLineItem representd Commit type. */
        Branch = 3, /**< The TimeLineItem representd Branch type. */
        Merge = 4 /**< The TimeLineItem representd Merge type. */
    };

    /**
      * Specialized ctor, creates a TimeLineItem object with the parameters supplied.
      * @param icon The icon that will be displayed in the TimeLine
      * @param dataList A QStringList with three elements:
      *     - dataList(0) contains the name that will be displayed under the icon;
      *     - dataList(1) contains the string that will be displayed in the popup;
      *     - dataList(2) contains the sha1 hash identifier.
      * @param id The object's identifier.
      * @param flag The object's flag.
      */
    TimeLineItem(const QIcon &icon,
                 QStringList &dataList,
                 const TimeLineItem::ItemIdentifier id,
                 const Qt::ItemFlag flag);

    /**
      * Sets the sha1 hash of its object.
      * @param hash The string that represents the hash.
      */
    void setHash(const QString &hash);

    /**
      * Sets the identifier of its object.
      * @param id The identifier that represents the hash.
      */
    void setIdentifier(const TimeLineItem::ItemIdentifier id);

    /**
      * @return The string containing the hash.
      */
    QString getHash();

    /**
      * @return The identifier.
      */
    TimeLineItem::ItemIdentifier getIdentifier();

private:
    QString        m_sha1hash;
    ItemIdentifier m_type;
};


#endif // TIMELINEITEM_H
