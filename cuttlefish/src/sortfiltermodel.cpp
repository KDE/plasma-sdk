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

#include "iconmodel.h"
#include "sortfiltermodel.h"

using namespace CuttleFish;

SortFilterModel::SortFilterModel(QObject *parent)
 : QSortFilterProxyModel(parent)
{
    setSortRole(IconModel::IconName);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}


bool SortFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{

    const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    if (!(m_category.isEmpty() || m_category == QLatin1String("all"))
        && (m_category != sourceIndex.data(IconModel::Category))) {
        return false;
    }
    if (!m_filter.isEmpty()) {
        return sourceIndex.data(IconModel::IconName).toString().contains(m_filter, Qt::CaseInsensitive);
    }
    return true;
}

int SortFilterModel::currentIndex()
{
    const QModelIndex index =  mapFromSource(m_currentSourceIndex);
    if (index.isValid()) {
        return index.row();
    }
    m_currentSourceIndex = mapToSource(this->index(0, 0));
    return 0;
}

void SortFilterModel::setCurrentIndex(int index)
{
    if (mapFromSource(m_currentSourceIndex).row() != index) {
        m_currentSourceIndex = mapToSource(this->index(index, 0));
        emit currentIndexChanged();
    }

}

QString SortFilterModel::category() const
{
    return m_category;
}

void SortFilterModel::setCategory(const QString &category)
{
    if (category == m_category) {
       return;
    }
    int oldIndex = currentIndex();
    m_category = category;
    invalidateFilter();
    emit categoryChanged();
    if (currentIndex() != oldIndex) {
        emit currentIndexChanged();
    }
}

QString SortFilterModel::filter() const
{
    return m_filter;
}

void SortFilterModel::setFilter(const QString& filter)
{
    if (filter == m_filter) {
        return;
    }
    int oldIndex = currentIndex();
    m_filter = filter;
    invalidateFilter();
    emit filterChanged();
    if (currentIndex() != oldIndex) {
        emit currentIndexChanged();
    }
}


