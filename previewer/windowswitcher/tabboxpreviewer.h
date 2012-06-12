/********************************************************************
 * This file is part of the KDE project.
 *
 * Copyright (C) 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef TABBOXPREVIEWER_H
#define TABBOXPREVIEWER_H

#include "../previewer.h"
#include "windowswitcher.h"

class TabBoxPreviewer : public Previewer {

    Q_OBJECT

public:

    TabBoxPreviewer(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    void showPreview(const QString &packagePath);

    /**
     * KWin::TabBox::LayoutPreview requires the fullpath of our main QML file in order to load the previewer
     * so here we retrieve the fullpath inside from the projectpath. For that task we will use the relative
     * path from our mail QML file, which is located inside the metadata.dekstop file.
     **/
    QString packageMainFile(const QString &projectpath);

    void setLayoutPath(const QString &filePath);

    void init();

    void refreshPreview();

public Q_SLOTS:

    void signalForward();

private:
    WindowSwitcher *m_view;
    QString m_fullFilenamePath;


};

#endif // TABBOXPREVIEWER_H