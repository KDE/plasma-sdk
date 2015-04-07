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

#ifndef COMMITSMODEL_H
#define COMMITSMODEL_H

#include <QIdentityProxyModel>
#include <vcs/vcsevent.h>

class Git;

class CommitsModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    CommitsModel(Git* parent);
    ~CommitsModel();

    enum Roles {
        Author = Qt::UserRole + 1,
        Date,
        Message
    };

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    Git *m_git;
    QList<KDevelop::VcsEvent> m_commitsList;
};

#endif
