/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
Copyright 2009-2010 Shantanu Tushar Jha <shaan7in@gmail.com>
Copyright 2009-2010 Sandro Andrade <sandroandrade@kde.org>
Copyright 2009-2010 Lim Yuen Hoe <yuenhoe@hotmail.com>

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

#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <QAbstractItemModel>

#include "packagehandler/plasmoidhandler.h"

class KDirWatch;

class PackageModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit PackageModel(PackageHandler *packageHandler, QObject *parent = 0);
    ~PackageModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    enum DataRoles {
        MimeTypeRole = Qt::UserRole + 1,
        UrlRole,
        PackagePathRole,
        ContentsWithSubdirRole
    };

Q_SIGNALS:
    void reloadModel();
private:
    QHash<QString, QStringList>  m_dialogOptions;
    PackageHandler *m_packageHandler;
    PackageHandler::Node* m_topNode;
    KDirWatch *m_watchDirectory;
    static const int MAX_COLUMN = 0;
};

#endif

