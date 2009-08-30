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

#include "timelinelistwidget.h"

#include <QModelIndex>
#include <QMouseEvent>
#include <QWidget>

#include <KDebug>

TimeLineListWidget::TimeLineListWidget(QWidget *parent)
        : QListWidget(parent)
{
}

TimeLineListWidget::~TimeLineListWidget()
{
}

void TimeLineListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable))
        return;

    QListWidget::mouseMoveEvent(event);
}

void TimeLineListWidget::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable)) {
        return;
    }

    QListWidget::mousePressEvent(event);
}

void TimeLineListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    //if ( index.isValid() && !( index.flags() & Qt::ItemIsSelectable ) )
    //return;

    QListWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        emit itemClicked(itemFromIndex(index));
    }
}

void TimeLineListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    kDebug();
    emit contextMenuRequested(itemAt(event->pos()));
}

#include "timelinelistwidget.moc"
