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


#include "previewcontainment.h"
#include "overlaytoolbox.h"

#include <QAction>
#include <QPainter>
#include <QGraphicsLayoutItem>

#include <Plasma/Label>
#include <KIcon>
#include <KDebug>


PreviewContainment::PreviewContainment(QObject *parent, const QVariantList &args)
    : Containment(parent, args), m_options(0), m_applet(0)
{
    connect(this, SIGNAL(appletAdded(Plasma::Applet *, const QPointF &)),
            this, SLOT(onAppletAdded(Plasma::Applet *, const QPointF &)));
    connect(this, SIGNAL(appletRemoved(Plasma::Applet *)),
            this, SLOT(onAppletRemoved(Plasma::Applet *)));

    setContainmentType(Plasma::Containment::CustomContainment);
    setBackgroundHints(Plasma::Applet::NoBackground);
    setHasConfigurationInterface(false);

    m_layout = new QGraphicsLinearLayout(Qt::Vertical);
    m_layout->setContentsMargins(5, 0, 5, 10);

    m_header = new QGraphicsLinearLayout(Qt::Horizontal);
    m_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_layout->addItem(m_header);
    setupHeader();
    setLayout(m_layout);
}

PreviewContainment::~PreviewContainment()
{
    if (m_options) {
        delete m_options;
    }
}

void PreviewContainment::setupHeader()
{
    // setup toolboxes
    form = new Plasma::IconWidget(this);

    refresh = new Plasma::IconWidget(this);
    refresh->setIcon(KIcon("user-desktop"));

    location = new Plasma::IconWidget(this);



}

    if (m_options) {
        m_options->setGeometry(geometry());
    }
}

void PreviewContainment::onAppletAdded(Plasma::Applet *applet, const QPointF &pos)
{
    Q_UNUSED(pos);
    m_layout->addItem(applet);
    connect(applet, SIGNAL(geometryChanged()), this, SLOT(onAppletGeometryChanged()));
}

void PreviewContainment::onAppletRemoved(Plasma::Applet *applet)
{
    disconnect(applet, SIGNAL(geometryChanged()), this, SLOT(onAppletGeometryChanged()));
    m_layout->removeItem(applet);
}

void PreviewContainment::onAppletGeometryChanged()
{
    // deal with sizing here.
}


K_EXPORT_PLASMA_APPLET(studiopreviewer, PreviewContainment)

