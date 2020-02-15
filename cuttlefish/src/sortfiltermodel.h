/***************************************************************************
 *                                                                         *
 *   Copyright 2020 David Redondo <kde@david-redondo.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/


#include <QSortFilterProxyModel>

namespace CuttleFish {

class SortFilterModel : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    SortFilterModel(QObject *parent);

    void setCategory(const QString &category);
    QString category() const;

    void setFilter(const QString &filter);
    QString filter() const;

    void setCurrentIndex(int index);
    int currentIndex();

Q_SIGNALS:
    void filterChanged();
    void categoryChanged();
    void currentIndexChanged();

private: 
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    QString m_category;
    QString m_filter;
    QModelIndex m_currentSourceIndex;
};

}
