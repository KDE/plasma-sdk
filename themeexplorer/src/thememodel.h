/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <kpackage/package.h>

namespace Plasma {
    class Theme;
}

class ThemeListModel;

class ThemeModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(ThemeListModel *themeList READ themeList CONSTANT)

public:
    enum Roles {
        ImagePath,
        Description,
        Delegate,
        UsesFallback,
        SvgAbsolutePath,
        IsWritable
    };

    explicit ThemeModel(const KPackage::Package &package, QObject *parent = 0);
    ~ThemeModel();

    ThemeListModel *themeList();

    virtual QHash<int, QByteArray> roleNames() const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void setTheme(const QString &theme);
    QString theme() const;

    void load();

    Q_INVOKABLE void editElement(const QString& imagePath);

Q_SIGNALS:
    void themeChanged();

private Q_SLOTS:
    void processFinished();

private:
    QHash<int, QByteArray> m_roleNames;

    Plasma::Theme *m_theme;
    QString m_themeName;
    KPackage::Package m_package;
    QJsonDocument m_jsonDoc;
    ThemeListModel *m_themeListModel;
};

#endif // THEMEMODEL_H
