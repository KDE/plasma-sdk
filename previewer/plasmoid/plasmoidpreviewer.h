/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
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
#include "plasmoidview.h"

class PlasmoidPreviewer : public Previewer {

    Q_OBJECT

signals:
    void refreshView(); // emitted to signal the containment to refresh

public:
    PlasmoidPreviewer(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    void showPreview(const QString &packagePath);
    void refreshPreview();

private:
    PlasmoidView* m_view;
};

#endif // PLASMOIDPREVIEWER_H