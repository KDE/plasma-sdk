/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2013 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
#include < *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MODELVIEWER
#define MODELVIEWER

#include <QDialog>
#include <QAbstractItemDelegate>
#include <QStyleOptionViewItem>

class QAbstractItemModel;
class QTreeView;

namespace Plasma
{
    class DataEngine;
} // namespace Plasma

class Delegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    Delegate(QObject *parent = 0);
    virtual ~Delegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

protected:
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
};

class ModelViewer : public QDialog
{
    Q_OBJECT

public:
    ModelViewer(Plasma::DataEngine *engine, const QString &m_source, QWidget *parent = 0);
    ~ModelViewer();

private Q_SLOTS:
    void engineDestroyed();

private:
    Plasma::DataEngine *m_engine;
    QString m_source;
    QAbstractItemModel *m_model;
    QTreeView *m_view;
};

#endif

