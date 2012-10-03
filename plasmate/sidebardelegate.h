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

#ifndef SIDEBARDELEGATE_H
#define SIDEBARDELEGATE_H

class SidebarDelegate : public QAbstractItemDelegate
{
public:
    SidebarDelegate(QObject *parent = 0);
    ~SidebarDelegate();

    void setShowText(bool show);
    bool isTextShown() const;

    // from QAbstractItemDelegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    bool m_showText;
};

#endif // SIDEBARDELEGATE_H
