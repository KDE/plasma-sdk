/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2009, 2011 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#ifndef WINDOWSWITCHER
#define WINDOWSWITCHER

#include <QAbstractListModel>
#include <QQuickView>
#include <QQuickImageProvider>

class WindowSwitcher : public QQuickView
{
    Q_OBJECT
public:
    WindowSwitcher(QWidget *parent = NULL);
    virtual ~WindowSwitcher();

    void setLayout(const QString &path, const QString &name = QString());
};

class TabBoxImageProvider : public QQuickImageProvider
{
public:
    TabBoxImageProvider(QAbstractListModel *model);
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
private:
    QAbstractListModel *m_model;
};

class ExampleClientModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ExampleClientModel(QObject *parent = NULL);
    virtual ~ExampleClientModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

private:
    void init();
    QStringList m_nameList;
};

#endif // WINDOWSWITCHER
