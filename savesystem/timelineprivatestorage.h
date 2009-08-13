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

#ifndef TIMELINEPRIVATESTORAGE_H
#define TIMELINEPRIVATESTORAGE_H

class TimeLineItem;
class TimeLineDelegate;
class TimeLineListWidget;

class QLabel;
class QSplitter;
class QHBoxLayout;
class QStackedWidget;

/**
  * This class is used as private storage for TimeLine class; it saves new items added,
  * and also set its visual representation as widget.
  */
class TimeLinePrivateStorage
{
public:
    TimeLinePrivateStorage();

    /**
      * Adjust the size of the widget, should be called every time a new item is added.
      * @param recalc Determines if recalculate the size by iterating every element stored.
      * @param expand Determines wether expand the width of the widget or not.
      */
    void adjustListSize(bool recalc, bool expand = true);

    TimeLineListWidget     *list;
    QSplitter              *splitter;
    QStackedWidget         *stack;
    QWidget                *sideContainer;
    QLabel                 *sideTitle;
    QHBoxLayout            *vlay;
    QWidget                *sideWidget;
    QWidget                *bottomWidget;
    QList< TimeLineItem* > pages;
    bool                   splitterSizesSet;
    int                    itemsHeight;
    TimeLineDelegate       *sideDelegate;
};

#endif // TIMELINEPRIVATESTORAGE_H
