/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
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

#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QTableWidgetItem>



class TimeLineItem : public QTableWidgetItem
{
public:
    enum ItemIdentifier {
        OutsideWorkingDir = 0, /**< The TimeLineItem represents an object that it isn't inside the working directory. */
        NotACommit = 1, /**< The TimeLineItem represents an object that is not yet a commit. */
        Commit = 2, /**< The TimeLineItem representd Commit type. */
        Branch = 3, /**< The TimeLineItem representd Branch type. */
        Merge = 4 /**< The TimeLineItem representd Merge type. */
    };

    TimeLineItem(const Qt::ItemFlag flag = Qt::ItemIsEnabled);

    void setHash(const QString &hash);
    void setIdentifier(const TimeLineItem::ItemIdentifier id);

    QString getHash();
    TimeLineItem::ItemIdentifier getIdentifier();

private:
    QString m_sha1hash;
    ItemIdentifier m_type;
};

#endif // TIMELINEITEM_H
