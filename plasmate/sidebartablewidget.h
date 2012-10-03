/*

Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
Copyright 2009-2010 Laurent Montel <montel@kde.org>

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

#ifndef SIDEBARTABLEWIDGET_H
#define SIDEBARTABLEWIDGET_H

#include <QTableWidget>
class SidebarItem;

class SidebarTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    SidebarTableWidget(Qt::DockWidgetArea location, QWidget *parent = 0);
    ~SidebarTableWidget();

    void addItem(SidebarItem *item);
    void clear();
    QSize scrollbarSize(bool vertical);
    Qt::DockWidgetArea location();
    int totalLenght();

public Q_SLOTS:
    void updateLayout(Qt::DockWidgetArea location);

protected:
    private:
    void updateSize(SidebarItem *item);
    void mouseReleaseEvent(QMouseEvent *event);

    QScrollBar *m_horizontalSB;
    QScrollBar *m_verticalSB;
    Qt::DockWidgetArea m_location;

    // Max Cell Dimension: according with its position, could be a width or height
    int m_maxCellDimension;

    // Total Length: stores the width/height of the entire row/column
    int m_totalLenght;

    // from QTableWidget
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);

    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};

#endif // SIDEBARTABLEWIDGET_H
