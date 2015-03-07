/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "commitsmodel.h"
#include "git.h"

#include <QDateTime>

CommitsModel::CommitsModel(Git* parent)
    : QIdentityProxyModel(parent),
      m_git(parent)
{
}

CommitsModel::~CommitsModel()
{
}

QVariant CommitsModel::data(const QModelIndex &index, int role) const
{
    int column;
    switch(role) {
        case Author:
            column = 2;
            break;
        case Date:
            column = 3;
            break;
        case Message:
            column = 1;
            break;
        default:
            return QVariant();
    }

    QModelIndex idx = sourceModel()->index(index.row(), column);
    return sourceModel()->data(idx, Qt::DisplayRole);
}

QHash<int, QByteArray> CommitsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[CommitsModel::Author] = "author";
    roleNames[CommitsModel::Date] = "CommitDate";
    roleNames[CommitsModel::Message] = "message";

    return roleNames;
}

