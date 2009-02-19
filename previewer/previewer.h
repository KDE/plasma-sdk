/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007 Aaron Seigo <aseigo@kde.org>
 * Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 * Copyright 2009 Artur Duque de Souza <morpheuz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef PREVIEWER_H
#define PREVIEWER_H

#include "previewcontainment.h"

#include <QGraphicsView>

#include <Plasma/Corona>

using namespace Plasma;

class Previewer : public QGraphicsView
{
    Q_OBJECT

public:
    Previewer(QWidget *parent = 0);
    void addApplet(const QString &name, const QVariantList &args = QVariantList());

private slots:
    void sceneRectChanged(const QRectF &rect);
    void resizeEvent(QResizeEvent *event);
    void appletRemoved();

private:
    Plasma::Corona m_corona;
    Plasma::FormFactor m_formfactor;
    Plasma::Containment *m_containment;
    Plasma::Applet *m_applet;
};

#endif //PREVIEWER_H
