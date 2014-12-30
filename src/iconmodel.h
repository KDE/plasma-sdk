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
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QStringList themes READ themes CONSTANT)
    Q_PROPERTY(QString plasmaTheme READ plasmaTheme WRITE setPlasmaTheme NOTIFY plasmaThemeChanged)

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

    void setCategory(const QString &cat);
    QString category() const;

    void setFilter(const QString &filter);
    QString filter() const;

    void setTheme(const QString &theme);
    QString theme() const;
    QStringList themes() const;

    void setPlasmaTheme(const QString &ptheme);
    QString plasmaTheme() const;

    void load();
    void update();


Q_SIGNALS:
    void filterChanged();
    void categoryChanged();
    void themeChanged();
    void plasmaThemeChanged();

private:
    QHash<int, QByteArray> m_roleNames;

    QStringList m_icons; // icon theme,
    QString m_category;
    QString m_theme;
    QStringList m_themes;
    QString m_plasmatheme;
    QHash<QString, QVariantMap> m_data; // icon theme,
};

} // namespace

#endif // ICONMODEL_H
