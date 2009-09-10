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

#ifndef TIMELINEDELEGATE_H
#define TIMELINEDELEGATE_H

#include <QAbstractItemDelegate>

class QModelIndex;
class QPainter;
class QSize;
class QStyleOptionViewItem;

/**
  * Simple delegate used to paint the icon and text of each TimeLineItem.
  */
class TimeLineDelegate : public QAbstractItemDelegate
{
public:

    /**
      * Default ctor.
      */
    TimeLineDelegate(QObject *parent = 0);
    ~TimeLineDelegate();

    /**
      * Sets whether the text will be displayed or not.
      * @param show The boolean value that will set the text displayed or not.
      */
    void setShowText(bool show);

    /**
      * @return True if the text will be displayed, or false otherwise.
      */
    bool isTextShown() const;

    /**
      * Overloads method from QAbstractItemDelegate, providing a custom painter for our
      * TimeLineItem objects.
      * @param painter Pointer to the QPainter object.
      * @param option Style option reference.
      * @param index Reference to the model index that will be painted.
      */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
      * @return The size of the painted object.
      */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    bool m_showText;
};

#endif // TIMELINEDELEGATE_H
