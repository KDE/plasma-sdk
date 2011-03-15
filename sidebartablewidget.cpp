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



#include <QModelIndex>
#include <QMouseEvent>
#include <QHeaderView>
#include <QFontMetrics>
#include <QScrollBar>

//#include <QDebug>

#include "sidebartablewidget.h"
#include "sidebaritem.cpp"

static const int ITEM_MARGIN_LEFT = 5;
static const int ITEM_MARGIN_TOP = 5;
static const int ITEM_MARGIN_RIGHT = 5;
static const int ITEM_MARGIN_BOTTOM = 5;
static const int ITEM_PADDING = 5;



SidebarTableWidget::SidebarTableWidget(Qt::DockWidgetArea location, QWidget *parent)
        : QTableWidget(0,0,parent),
          m_horizontalSB(0),
          m_verticalSB(0),
          m_location(location),
          m_maxCellDimension(0),
          m_totalLenght(0)
{
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    setIconSize(QSize(32,32));
    setShowGrid(false);
    setMouseTracking(true);
    viewport()->setAutoFillBackground(false);
    setSelectionMode(QAbstractItemView::SingleSelection);

    m_horizontalSB = horizontalScrollBar();
    m_verticalSB = verticalScrollBar();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

SidebarTableWidget::~SidebarTableWidget()
{
}

Qt::DockWidgetArea SidebarTableWidget::location()
{
    return m_location;
}

void SidebarTableWidget::updateLayout(Qt::DockWidgetArea location)
{
    bool wasVertical = true;

    // Ensure we REALLY need to update the layout
    switch(location) {
        case Qt::LeftDockWidgetArea:
            if((m_location == Qt::LeftDockWidgetArea)||
               (m_location == Qt::RightDockWidgetArea)) {
                return;
            }
            wasVertical = false;
        break;

        case Qt::RightDockWidgetArea:
            if((m_location == Qt::LeftDockWidgetArea)||
               (m_location == Qt::RightDockWidgetArea)) {
                return;
            }
            wasVertical = false;
        break;

        case Qt::TopDockWidgetArea:
            if((m_location == Qt::TopDockWidgetArea)||
               (m_location == Qt::BottomDockWidgetArea)) {
                return;
            }
        break;

        case Qt::BottomDockWidgetArea:
            if((m_location == Qt::TopDockWidgetArea)||
               (m_location == Qt::BottomDockWidgetArea)) {
                return;
            }
        break;

        default:
            return;
    }
    // Before starting swapping, hide the table for a while to avoid flickering and reset
    // minimum and maximum widget size;
    hide();

    // Store the current position
    m_location = location;

    // Compute the number of required iterations
    int iterations = (wasVertical ? rowCount() : columnCount());

    if(wasVertical) {
        setColumnCount(iterations);
    } else {
        setRowCount(iterations);
    }

    m_maxCellDimension = 0;
    m_totalLenght = 0;

    // Save position of the selected item, if exists
    int position = 0;
    QTableWidgetItem *it = currentItem();
    if(it) {
        position = (wasVertical ? it->row() : it->column());
    }

    for(int i=0; i<iterations; ++i) {
        QTableWidgetItem *tItem =takeItem((wasVertical ? i : 0) ,
                                          (wasVertical ? 0 : i));

        if(!tItem)
            return;

        setItem((wasVertical ? 0 : i),
                (wasVertical ? i : 0),
                tItem);

        updateSize((SidebarItem*)tItem);

    }

    // Restore the selected item
    setCurrentCell(wasVertical? 0: position,
                   wasVertical? position: 0);

    // Remove unwanted columns or rows, then display the widget again
    if(wasVertical) {
        setRowCount(1);
    } else {
        setColumnCount(1);
    }

    show();
}

QSize SidebarTableWidget::scrollbarSize(bool vertical)
{
    return (vertical ? m_verticalSB->size() : m_horizontalSB->size());
}

void SidebarTableWidget::addItem(SidebarItem *item)
{
    // According with the orientation, expand the widget through the first column or row,
    // then add the item.
    setRowCount(((m_location == Qt::RightDockWidgetArea)||(m_location == Qt::LeftDockWidgetArea)) ?
                rowCount()+1 : 1);
    setColumnCount(((m_location == Qt::RightDockWidgetArea)||(m_location == Qt::LeftDockWidgetArea)) ?
                   1 : columnCount()+1);

    setItem(((m_location == Qt::RightDockWidgetArea)||(m_location == Qt::LeftDockWidgetArea)) ?
            rowCount()-1 : 0,
            ((m_location == Qt::RightDockWidgetArea)||(m_location == Qt::LeftDockWidgetArea)) ?
            0 : columnCount()-1,
            item);

    updateSize(item);
}

int SidebarTableWidget::totalLenght()
{
    return m_totalLenght;
}

void SidebarTableWidget::clear()
{
    bool vertical = ((m_location == Qt::RightDockWidgetArea)||(m_location == Qt::LeftDockWidgetArea));
    for(int i = 0; i < (vertical ? rowCount(): columnCount()); ++i ) {
        takeItem(vertical ? i: 0,
                 vertical ? 0: i);
    }
    setRowCount(0);
    setColumnCount(0);

    m_maxCellDimension = 0;
    m_totalLenght = 0;
}

/*void SidebarTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;
    if (event->button() == Qt::RightButton) {
        emit itemClicked(itemFromIndex(index));
    }
}*/

void SidebarTableWidget::updateSize(SidebarItem *item)
{
    // Now compute the correct row and column size for the current item
    QFontMetrics font(item->font());
    QSize rect(ITEM_MARGIN_LEFT + qMax(iconSize().width(), font.width(item->text() + 2*ITEM_PADDING)) + ITEM_MARGIN_RIGHT,
               ITEM_MARGIN_TOP + iconSize().height() + ITEM_PADDING +
               font.boundingRect(item->text()).height() + 2*ITEM_MARGIN_BOTTOM );


    //qDebug() << "SidebarTableWidget::updateSize = " << rect;

    if((m_location == Qt::RightDockWidgetArea) || (m_location == Qt::LeftDockWidgetArea)) {
        setRowHeight(item->row(),rect.height());
        m_totalLenght += rect.height();

        if(m_maxCellDimension < rect.width()) {
            m_maxCellDimension = rect.width();
            setColumnWidth(1,m_maxCellDimension);
        }
        setMinimumHeight(0);
        setMaximumHeight(16777215);
        setColumnWidth(1,m_maxCellDimension);
        setColumnWidth(0,m_maxCellDimension);
        setMinimumWidth(m_maxCellDimension+5);
        setMaximumWidth(m_maxCellDimension+5);

    } else {
        setColumnWidth(item->column(),rect.width());
        m_totalLenght += rect.width();

        if(m_maxCellDimension < rect.height()) {
            m_maxCellDimension = rect.height();
            setRowHeight(1,m_maxCellDimension);
        }
        setMinimumWidth(0);
        setMaximumWidth(16777215);
        setRowHeight(1,m_maxCellDimension);
        setRowHeight(0,m_maxCellDimension);
        setMinimumHeight(m_maxCellDimension+5);
        setMaximumHeight(m_maxCellDimension+5);
    }
}


void SidebarTableWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable)) {
        return;
    }

    QTableWidget::mouseDoubleClickEvent(event);
}

void SidebarTableWidget::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable)) {
        return;
    }

    QTableWidget::mouseMoveEvent(event);
}

void SidebarTableWidget::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable)) {
        return;
    }

    QTableWidget::mousePressEvent(event);
}

void SidebarTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && !(index.flags() & Qt::ItemIsSelectable)) {
        return;
    }

    QTableWidget::mouseReleaseEvent(event);
}

QModelIndex SidebarTableWidget::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    QModelIndex oldindex = currentIndex();
    QModelIndex newindex = oldindex;
    switch (cursorAction) {
    case MoveUp:
    case MovePrevious: {
        int row = oldindex.row() - 1;
        while (row > -1 && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) --row;
        if (row > -1)
            newindex = model()->index(row, 0);
        break;
    }
    case MoveDown:
    case MoveNext: {
        int row = oldindex.row() + 1;
        int max = model()->rowCount();
        while (row < max && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) ++row;
        if (row < max)
            newindex = model()->index(row, 0);
        break;
    }
    case MoveHome:
    case MovePageUp: {
        int row = 0;
        while (row < oldindex.row() && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) ++row;
        if (row < oldindex.row())
            newindex = model()->index(row, 0);
        break;
    }
    case MoveEnd:
    case MovePageDown: {
        int row = model()->rowCount() - 1;
        while (row > oldindex.row() && !(model()->index(row, 0).flags() & Qt::ItemIsSelectable)) --row;
        if (row > oldindex.row())
            newindex = model()->index(row, 0);
        break;
    }
    // no navigation possible for these
    case MoveLeft:
    case MoveRight:
        break;
    }

    // dirty hack to change item when the key cursor changes item
    if (oldindex != newindex) {
        emit itemClicked(itemFromIndex(newindex));
    }

    return newindex;
}

