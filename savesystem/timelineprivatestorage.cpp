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
#include "timelinedelegate.h"
#include "timelinelistwidget.h"
#include "timelineprivatestorage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QSplitter>
#include <QStackedWidget>
#include <QWidget>



TimeLinePrivateStorage::TimeLinePrivateStorage()
        : sideWidget(0), bottomWidget(0), splitterSizesSet(false),
        itemsHeight(0)
{
}

void TimeLinePrivateStorage::adjustListSize(bool recalc, bool expand)
{
    QRect bottomElemRect(
        QPoint(0, 0),
        list->sizeHintForIndex(list->model()->index(list->count() - 1, 0))
    );
    if (recalc) {
        int w = 0;
        for (int i = 0; i < list->count(); ++i) {
            QSize s = list->sizeHintForIndex(list->model()->index(i, 0));
            if (s.width() > w)
                w = s.width();
        }
        bottomElemRect.setWidth(w);
    }
    bottomElemRect.translate(0, bottomElemRect.height() *(list->count() - 1));
    itemsHeight = bottomElemRect.height() * list->count();
    //list->setMinimumHeight( itemsHeight + list->frameWidth() * 2 );
    int curWidth = list->minimumWidth();
    int newWidth = expand
                   ? qMax(bottomElemRect.width() + list->frameWidth() * 2, curWidth)
                   : qMin(bottomElemRect.width() + list->frameWidth() * 2, curWidth);
    list->setFixedWidth(newWidth);
}
