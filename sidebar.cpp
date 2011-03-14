/*
Copyright 2007 by Diego [Po]lentino Casella <polentino911@gmail.com>     *
                                                                          *
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KConfig>
#include <KIcon>
#include <KLocalizedString>

#include <QDockWidget>
#include <QModelIndex>
#include <QResizeEvent>
#include <QString>

//#include <QDebug>

#include "sidebar.h"
#include "sidebartablewidget.h"
#include "sidebardelegate.h"
#include "sidebaritem.cpp"

Sidebar::Sidebar(QWidget *parent, Qt::DockWidgetArea location)
        :QDockWidget(i18n("Workflow"))
{
    Q_UNUSED(parent)

    setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_table = new SidebarTableWidget(location);
    m_table->setParent(this);
    setWidget(m_table);

    m_delegate = new SidebarDelegate();
    m_table->setItemDelegate(m_delegate);

    connect(m_table, SIGNAL(clicked(const QModelIndex &)),
            this, SIGNAL(currentIndexClicked(const QModelIndex &)));

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            m_table,SLOT(updateLayout(Qt::DockWidgetArea)));

}

Sidebar::~Sidebar()
{
    //delete d;
}

Qt::DockWidgetArea Sidebar::location()
{
    return m_table->location();
}

void Sidebar::resizeEvent(QResizeEvent * event)
{
    /*
      I've reimplemented this event handler because, when resizing the main widget to a size
      smaller than the dockwidget, a bothersome effects appears. According with the orientation,
      it appears a scrollbar INSIDE the m_table widget, which hovers partially the icons when
      the dock is vertical, and hovers completely the text when it is horizontal.
      So I've implemented this simple workaround, in order to expand the dockwidget when a
      scrollbar is shown.
      I know that resizing a child widget inside a parent resizeEvent() is dangerous at 98%,
      by the way this trick is very simple and well structured, so it wont cause issues I hope =)
      */

    QSize newSize = event->size();
    //qDebug() << "New size: " << newSize;
    //qDebug() << "m_table size:" << m_table->size();
    //qDebug() << "m_table total lenght" << m_table->totalLenght();

    bool vertical = ((m_table->location() == Qt::RightDockWidgetArea) ||
                    (m_table->location() == Qt::LeftDockWidgetArea));

    if(vertical) {
        if(m_table->totalLenght() > newSize.height()) {
            m_table->setFixedWidth(m_table->columnWidth(0) + m_table->scrollbarSize(vertical).width());
            return;
        } else {
            if(m_table->columnWidth(0) < newSize.width()) {
                m_table->setFixedWidth(m_table->columnWidth(0));
                return;
            }
        }
    } else {
        if(m_table->totalLenght() > newSize.width()) {
            m_table->setFixedHeight(m_table->rowHeight(0) + m_table->scrollbarSize(vertical).height());
            return;
        } else {
            if(m_table->rowHeight(0) < newSize.height()) {
                m_table->setFixedHeight(m_table->rowHeight(0));
                return;
            }
        }
    }
}

void Sidebar::addItem(const QIcon &icon, const QString &text)
{
    SidebarItem *newItem = new SidebarItem(icon, text);
    m_table->addItem(newItem);
}

bool Sidebar::isVertical()
{
    return ((m_table->location() == Qt::LeftDockWidgetArea)||(m_table->location() == Qt::RightDockWidgetArea));
}

bool Sidebar::isItemEnabled(int index) const
{
    /*if (index < 0 || index >= d->pages.count())
        return false;

    Qt::ItemFlags f = d->pages.at(index)->flags();
    return (f & Qt::ItemIsEnabled) == Qt::ItemIsEnabled;*/
    return true;
}

void Sidebar::setCurrentIndex(int index)
{
    /*if (index < 0 || index >= d->pages.count() || !isItemEnabled(index))
        return;*/
    bool vertical = ((m_table->location() == Qt::LeftDockWidgetArea)||
                     (m_table->location() == Qt::RightDockWidgetArea));

    QModelIndex modelindex = m_table->model()->index(vertical? 0: index,
                                                     vertical? index: 0);
    m_table->setCurrentIndex(modelindex);
    m_table->selectionModel()->select(modelindex, QItemSelectionModel::ClearAndSelect);
}

int Sidebar::currentIndex() const
{
    return m_table->currentRow();
}

void Sidebar::setSidebarVisibility(bool visible)
{
    if (visible != m_table->isHidden())
        return;

    static bool sideWasVisible = !isHidden();

    m_table->setHidden(!visible);
    if (visible) {
        setHidden(!sideWasVisible);
        sideWasVisible = true;
    } else {
        sideWasVisible = !isHidden();
        setHidden(true);
    }
}

bool Sidebar::isSidebarVisible() const
{
    return !isHidden();
}

#include "sidebar.moc"
