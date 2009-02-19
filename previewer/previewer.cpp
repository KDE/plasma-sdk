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

#include "previewer.h"
#include "previewcontainment.h"

#include <Plasma/Containment>

Previewer::Previewer(QWidget *parent)
    : QGraphicsView(parent),
      m_containment(0),
      m_applet(0)
{
    setScene(&m_corona);
    connect(&m_corona, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    setAlignment(Qt::AlignCenter);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // containment stuff (must go to constructor / init ?)
    m_containment = m_corona.addContainment("studiopreviewer");
    m_containment->setFormFactor(Plasma::Planar);
    m_containment->setLocation(Plasma::Floating);
    connect(m_containment, SIGNAL(appletRemoved(Plasma::Applet*)), this, SLOT(appletRemoved()));

    setScene(m_containment->scene());
}

void Previewer::addApplet(const QString &name, const QVariantList &args)
{
    m_applet = m_containment->addApplet(name, args, QRectF(0, 0, -1, -1));
    m_applet->setFlag(QGraphicsItem::ItemIsMovable, false);
    resize(m_applet->preferredSize().toSize());
}

void Previewer::appletRemoved()
{
    m_applet = 0;
}

void Previewer::sceneRectChanged(const QRectF &rect)
{
    Q_UNUSED(rect);

    if (m_containment) {
        setSceneRect(m_containment->geometry());
    }
}

void Previewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    // if we do not have any applet being shown
    // there is no need to do all this stuff
    if (!m_applet) {
        m_containment->resize(size());
        return;
    }

    qreal newWidth = 0;
    qreal newHeight = 0;

    if (m_applet->aspectRatioMode() == Plasma::KeepAspectRatio) {
        // The applet always keeps its aspect ratio, so let's respect it.
        qreal ratio = m_applet->size().width() / m_applet->size().height();
        qreal widthForCurrentHeight = (qreal)size().height() * ratio;
        if (widthForCurrentHeight > size().width()) {
            newHeight = size().width() / ratio;
            newWidth = newHeight * ratio;
        } else {
            newWidth = widthForCurrentHeight;
            newHeight = newWidth / ratio;
        }
    } else {
        newWidth = size().width();
        newHeight = size().height();
    }
    QSizeF newSize(newWidth, newHeight);

    // check if the rect is valid, or else it seems to try to allocate
    // up to infinity memory in exponential increments
    if (newSize.isValid()) {
        m_containment->resize(QSizeF(newWidth, newHeight));
    }
}

