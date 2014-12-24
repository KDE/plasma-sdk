/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
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

#ifndef ICONMODEL_H
#define ICONMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>

namespace CuttleFish {


class IconModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    enum Roles {
        FileName = Qt::UserRole + 1,
        IconName,
        Icon,
        FullPath,
        Category,
        Size
    };

    explicit IconModel(QObject *parent = 0);


    virtual QHash<int, QByteArray> roleNames() const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QString key(int role) const;

    void add(const QFileInfo &info);
    void remove(const QString &iconFile);

    void setFilter(const QString &filter);
    QString filter() const;

    void load();
    void update();


Q_SIGNALS:
    void filterChanged();

private:
    QHash<int, QByteArray> m_roleNames;

    QStringList m_icons; // icon theme,
    QHash<QString, QVariantMap> m_data; // icon theme,
};

} // namespace

#endif // ICONMODEL_H
