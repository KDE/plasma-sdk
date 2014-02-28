/*
 *   Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *   Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>
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

#ifndef PLASMOIDPREVIEWER_H
#define PLASMOIDPREVIEWER_H

#include "../previewer.h"

#include <QScopedPointer>

class View;

class PlasmoidPreviewer : public Previewer {

    Q_OBJECT

public:
    PlasmoidPreviewer(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0 );
    ~PlasmoidPreviewer();

    void showPreview(const QString &packagePath);
    void refreshPreview();

Q_SIGNALS:
    void refreshView();

private:
    QScopedPointer<View> m_view;
};

#endif // PLASMOIDPREVIEWER_H

