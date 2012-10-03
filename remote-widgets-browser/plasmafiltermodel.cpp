/* Copyright 2011 Kevin Ottens <ervin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.
*/

#include "plasmafiltermodel.h"

#include <KDirModel>
#include <KRun>

PlasmaFilterModel::PlasmaFilterModel(QObject *parent)
    : KRecursiveFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

PlasmaFilterModel::~PlasmaFilterModel()
{
}

bool PlasmaFilterModel::filterAcceptsColumn(int sourceColumn, const QModelIndex&) const
{
    return sourceColumn==0;
}

bool PlasmaFilterModel::acceptRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceChild = sourceModel()->index(sourceRow, 0, sourceParent);

    if (sourceModel()->canFetchMore(sourceChild)) {
        sourceModel()->fetchMore(sourceChild);
    }

    KFileItem file = sourceChild.data(KDirModel::FileItemRole).value<KFileItem>();
    return file.targetUrl().protocol()=="plasma";
}

void PlasmaFilterModel::runIndex(const QModelIndex &index)
{
    KFileItem file = index.data(KDirModel::FileItemRole).value<KFileItem>();
    new KRun(file.targetUrl(), 0);
}

