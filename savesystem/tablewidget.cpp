/*

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
#include <QModelIndex>
#include <QMouseEvent>
#include <QHeaderView>
#include <QFontMetrics>
#include <QScrollBar>

//#include <QDebug>

#include "tablewidget.h"
#include "timelineitem.h"

static const int ITEM_MARGIN_LEFT = 5;
static const int ITEM_MARGIN_TOP = 5;
static const int ITEM_MARGIN_RIGHT = 5;
static const int ITEM_MARGIN_BOTTOM = 5;
static const int ITEM_PADDING = 5;

TableWidget::TableWidget(Qt::DockWidgetArea location, QWidget *parent)
        :QTableWidget(0,0,parent),
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

    m_horizontalSB = horizontalScrollBar();
    m_verticalSB = verticalScrollBar();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //updateLayout();
}

Qt::DockWidgetArea TableWidget::location()
{
    return m_location;
}

void TableWidget::updateLayout(Qt::DockWidgetArea location)
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

    for(int i=0; i<iterations; ++i) {
        QTableWidgetItem *tItem =takeItem((wasVertical ? i : 0) ,
                                          (wasVertical ? 0 : i));

        if(!tItem)
            return;

        setItem((wasVertical ? 0 : i),
                (wasVertical ? i : 0),
                tItem);

        updateSize((TimeLineItem*)tItem);

    }

    // Remove unwanted columns or rows, then display the widget again
    if(wasVertical) {
        setRowCount(1);
    } else {
        setColumnCount(1);
    }

    show();
}

QSize TableWidget::scrollbarSize(bool vertical)
{
    return (vertical ? m_verticalSB->size() : m_horizontalSB->size());
}

void TableWidget::addItem(TimeLineItem *item)
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

int TableWidget::totalLenght()
{
    return m_totalLenght;
}

void TableWidget::clear()
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

void TableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()){
        return;
    }
    emit itemClicked(itemFromIndex(index));
}

void TableWidget::updateSize(TimeLineItem *item)
{
    // Now compute the correct row and column size for the current item
    QFontMetrics font(item->font());
    QSize rect(ITEM_MARGIN_LEFT + qMax(iconSize().width(), font.width(item->text())) + ITEM_MARGIN_RIGHT,
               ITEM_MARGIN_TOP + iconSize().height() + ITEM_PADDING +
               font.boundingRect(item->text()).height() + 2*ITEM_MARGIN_BOTTOM );


    //qDebug() << "TableWidget::updateSize = " << rect;

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
