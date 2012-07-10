/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "plasmoidpreviewer.h"

#include <QFile>

PlasmoidPreviewer::PlasmoidPreviewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
        : Previewer(title, parent, flags)
{
    m_view = new PlasmoidView(this);
    setWidget(m_view);
}

void PlasmoidPreviewer::showPreview(const QString &packagePath)
{
    m_view->addApplet(packagePath);
}

void PlasmoidPreviewer::refreshPreview()
{
    emit refreshView();
}

QString PlasmoidPreviewer::takeOutput() const
{
    QFile file("/var/tmp/plasmatepreviewerlog.txt");
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }

    return QString();
}
