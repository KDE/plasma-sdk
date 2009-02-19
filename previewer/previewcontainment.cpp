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

#include <QPainter>
#include <QGraphicsLayoutItem>

#include <Plasma/Label>
#include <KIcon>
#include <KAction>


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

    m_tmp = new QGraphicsWidget();
    m_layout->addItem(m_tmp);
    m_layout->setStretchFactor(m_tmp, 2);

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

    // add actions
    KAction *action1 = new KAction(KIcon("list-add"), "", this);
    connect(action1, SIGNAL(triggered()), this, SLOT(changeFormFactor()));
    form->setAction(action1);

    KAction *action2 = new KAction(KIcon("list-add"), "", this);
    connect(action2, SIGNAL(triggered()), this, SLOT(changeLocation()));
    location->setAction(action2);

    // add the toolboxes
    m_header->addItem(form);
    m_header->addItem(refresh);
    m_header->addItem(location);
}

void PreviewContainment::changeFormFactor()
{
    if (m_options) {
        delete m_options;
    }

    m_options = new OverlayToolBox(this);
    m_options->setGeometry(geometry());
    m_options->setZValue(100000000);

    KAction *planar = new KAction(KIcon("user-desktop"),
                                  i18n("Planar"), m_options);
    KAction *mediacenter = new KAction(KIcon("user-desktop"),
                                       i18n("Media Center"), m_options);
    KAction *horizontal = new KAction(KIcon("user-desktop"),
                                      i18n("Horizontal"), m_options);
    KAction *vertical = new KAction(KIcon("user-desktop"),
                                    i18n("Vertical"), m_options);
    KAction *cancel = new KAction(KIcon("dialog-cancel"),
                                  i18n("Cancel"), m_options);

    m_options->addTool(planar);
    m_options->addTool(mediacenter);
    m_options->addTool(horizontal);
    m_options->addTool(vertical);
    m_options->addTool(cancel);

    connect(planar, SIGNAL(triggered(bool)), this, SLOT(planarTriggered(bool)));
    connect(mediacenter, SIGNAL(triggered(bool)), this, SLOT(mediacenterTriggered(bool)));
    connect(horizontal, SIGNAL(triggered(bool)), this, SLOT(horizontalTriggered(bool)));
    connect(vertical, SIGNAL(triggered(bool)), this, SLOT(verticalTriggered(bool)));
    connect(cancel, SIGNAL(triggered(bool)), this, SLOT(cancelOption(bool)));

    m_options->show();
}

void PreviewContainment::planarTriggered(bool)
{
    m_options->hide();
    setFormFactor(Plasma::Planar);
}

void PreviewContainment::mediacenterTriggered(bool)
{
    m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::horizontalTriggered(bool)
{
    m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::verticalTriggered(bool)
{
    m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::changeLocation()
{
    if (m_options) {
        delete m_options;
    }

    m_options = new OverlayToolBox(this);
    m_options->setGeometry(geometry());
    m_options->setZValue(100);

    KAction *floating = new KAction(KIcon("user-trash"),
                                    i18n("Floating"), m_options);
    KAction *desktop = new KAction(KIcon("user-trash"),
                                   i18n("Desktop"), m_options);
    KAction *fullscreen = new KAction(KIcon("user-trash"),
                                      i18n("FullScreen"), m_options);
    KAction *topEdge = new KAction(KIcon("user-trash"),
                                   i18n("Top Edge"), m_options);
    KAction *bottomEdge = new KAction(KIcon("user-trash"),
                                      i18n("Bottom Edge"), m_options);
    KAction *leftEdge = new KAction(KIcon("user-trash"),
                                    i18n("Left Edge"), m_options);
    KAction *rightEdge = new KAction(KIcon("user-trash"),
                                     i18n("Right Edge"), m_options);
    KAction *cancel = new KAction(KIcon("dialog-cancel"),
                                  i18n("Cancel"), m_options);

    m_options->addTool(floating);
    m_options->addTool(desktop);
    m_options->addTool(fullscreen);
    m_options->addTool(topEdge);
    m_options->addTool(bottomEdge);
    m_options->addTool(leftEdge);
    m_options->addTool(rightEdge);
    m_options->addTool(cancel);

    connect(floating, SIGNAL(triggered(bool)), this, SLOT(floatingTriggered(bool)));
    connect(desktop, SIGNAL(triggered(bool)), this, SLOT(desktopTriggered(bool)));
    connect(fullscreen, SIGNAL(triggered(bool)), this, SLOT(fullscreenTriggered(bool)));
    connect(topEdge, SIGNAL(triggered(bool)), this, SLOT(topEdgeTriggered(bool)));
    connect(bottomEdge, SIGNAL(triggered(bool)), this, SLOT(bottomEdgeTriggered(bool)));
    connect(leftEdge, SIGNAL(triggered(bool)), this, SLOT(leftEdgeTriggered(bool)));
    connect(rightEdge, SIGNAL(triggered(bool)), this, SLOT(rightEdgeTriggered(bool)));
    connect(cancel, SIGNAL(triggered(bool)), this, SLOT(cancelOption(bool)));

    m_options->show();
}

void PreviewContainment::floatingTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::Desktop);
}

void PreviewContainment::desktopTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::Desktop);
}

void PreviewContainment::fullscreenTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::FullScreen);
}

void PreviewContainment::topEdgeTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::TopEdge);
}

void PreviewContainment::bottomEdgeTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::BottomEdge);
}

void PreviewContainment::leftEdgeTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::LeftEdge);
}

void PreviewContainment::rightEdgeTriggered(bool)
{
    m_options->hide();
    setLocation(Plasma::RightEdge);
}

void PreviewContainment::cancelOption(bool)
{
    m_options->hide();
}

void PreviewContainment::constraintsEvent(Plasma::Constraints constraints)
{
    Q_UNUSED(constraints);

    if (constraints == Plasma::SizeConstraint) {
        if (m_options) {
            m_options->setGeometry(geometry());
        }
    }
}

void PreviewContainment::onAppletAdded(Plasma::Applet *applet, const QPointF &pos)
{
    Q_UNUSED(pos);
    m_layout->removeItem(m_tmp);

    m_layout->addItem(applet);
    connect(applet, SIGNAL(geometryChanged()), this, SLOT(onAppletGeometryChanged()));
}

void PreviewContainment::onAppletRemoved(Plasma::Applet *applet)
{
    disconnect(applet, SIGNAL(geometryChanged()), this, SLOT(onAppletGeometryChanged()));
    m_layout->removeItem(applet);

    m_layout->addItem(m_tmp);
    m_layout->setStretchFactor(m_tmp, 2);
}

void PreviewContainment::onAppletGeometryChanged()
{
    // deal with sizing here.
}


K_EXPORT_PLASMA_APPLET(studiopreviewer, PreviewContainment)

