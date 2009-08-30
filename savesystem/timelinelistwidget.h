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

#ifndef TIMELINELISTWIDGET_H
#define TIMELINELISTWIDGET_H

#include    <QListWidget>

/**
  * Class that implements a custom list widget that ignores events for disabled items.
  */
class TimeLineListWidget : public QListWidget
{
    Q_OBJECT

public:
    /**
      * Default ctor, actually it does nothing =)
      */
    TimeLineListWidget(QWidget *parent = 0);
    ~TimeLineListWidget();

Q_SIGNALS:
    void contextMenuRequested(QListWidgetItem*);

protected:

    /**
      * Overloads method from QWidget; it checks the item that emits the event is
      * enabled and, if so, fires a QWidget::mouseMoveEvent().
      * @param event The mouse event triggered by the user interaction.
      */
    void mouseMoveEvent(QMouseEvent *event);

    /**
      * Overloads method from QWidget; it checks the item that emits the event is
      * enabled and, if so, fires a QWidget::mousePressEvent().
      * @param event The mouse event triggered by the user interaction.
      */
    void mousePressEvent(QMouseEvent *event);

    /**
      * Overloads method from QWidget; it checks the item that emits the event is
      * enabled and, if so, fires a QWidget::mouseReleaseEvent().
      * @param event The mouse event triggered by the user interaction.
      */
    void mouseReleaseEvent(QMouseEvent *event);

    void contextMenuEvent(QContextMenuEvent *event);
};

#endif // TIMELINELISTWIDGET_H
