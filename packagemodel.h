/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
Copyright 2009-2010 Shantanu Tushar Jha <jhahoneyk@gmail.com>
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

#include <Plasma/PackageStructure>

class KDirWatch;

namespace Plasma
{
class Package;
} // namespace Plasma

class PackageModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit PackageModel(QObject *parent = 0);
    ~PackageModel();

    void setPackageType(const QString &type);
    QString packageType() const;

    QString implementationApi() const;

    int setPackage(const QString &path);
    QString package() const;
    QString contentsPrefix() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    enum DataRoles { MimeTypeRole = Qt::UserRole + 1, UrlRole };

private:
    int loadPackage();

private Q_SLOTS:
    void fileAddedOnDisk(const QString &path);
    void fileDeletedOnDisk(const QString &path);

private:
    KDirWatch *m_directory;
    QVector<const char *> m_topEntries;
    QHash<const char *, QStringList> m_files;
    QHash<const char *, QList<const char *> > m_namedFiles;
    Plasma::PackageStructure::Ptr m_structure;
    Plasma::Package *m_package;
};

#endif

