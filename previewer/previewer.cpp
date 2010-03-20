/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "previewer.h"

Previewer::Previewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags)
{
}

Previewer::Previewer(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
}

void Previewer::emitRefreshRequest()
{
    emit refreshRequested();
}