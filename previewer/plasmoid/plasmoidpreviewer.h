/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef PLASMOIDPREVIEWER_H
#define PLASMOIDPREVIEWER_H

#include "../previewer.h"
#include "plasmoidview.h"

// This is the base previewer dockwidget class
// All previewer types should inherit from this class
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

#endif // PREVIEWER_H