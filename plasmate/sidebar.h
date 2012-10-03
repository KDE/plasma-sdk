/*
Copyright 2007 by Pino Toscano <pino@kde.org>

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


#ifndef _SIDEBAR_H_
#define _SIDEBAR_H_

#include <QDockWidget>

class QIcon;
class SidebarTableWidget;
class SidebarDelegate;

class Sidebar : public QDockWidget
{
    Q_OBJECT
public:
    Sidebar(QWidget *parent = 0,
            Qt::DockWidgetArea location = Qt::TopDockWidgetArea);
    ~Sidebar();

    Qt::DockWidgetArea location();
    void addItem(const QIcon &icon, const QString &text);

    bool isItemEnabled(int index) const;

    void setCurrentIndex(int index);
    int currentIndex() const;

    void setSidebarVisibility(bool visible);
    bool isSidebarVisible() const;

    bool isVertical();

signals:
    void currentIndexClicked(const QModelIndex &item);

protected:
    void resizeEvent(QResizeEvent * event);

private:
    void saveSplitterSize() const;

    SidebarTableWidget *m_table;
    SidebarDelegate *m_delegate;
};

#endif
