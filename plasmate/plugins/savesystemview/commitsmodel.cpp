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
    : QAbstractListModel(parent),
      m_git(parent)
{
}

CommitsModel::~CommitsModel()
{
}

QVariant CommitsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_commitsList.size()) {
        return QVariant();
    }

    switch(role) {
        case Author:
            return m_commitsList.at(index.row()).author();
        case Date:
            return m_commitsList.at(index.row()).date();
        case Message:
            return m_commitsList.at(index.row()).message();
        default:
            return QVariant();
    }
}

int CommitsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_commitsList.size();
}

QHash<int, QByteArray> CommitsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[CommitsModel::Author] = "author";
    roleNames[CommitsModel::Date] = "CommitDate";
    roleNames[CommitsModel::Message] = "message";

    return roleNames;
}

void CommitsModel::resetModel()
{
    beginResetModel();
    m_commitsList.clear();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, m_git->commits().size() - 1);
    m_commitsList = m_git->commits();
    endInsertRows();
}

