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
#include <QSignalMapper>

#include <Plasma/Label>
#include <Plasma/Wallpaper>
#include <Plasma/Package>
#include <KIcon>
#include <KAction>


PreviewContainment::PreviewContainment(QObject *parent, const QVariantList &args)
    : Containment(parent, args), m_options(0), m_applet(0), m_menu(0), m_menutype(OVERLAY)
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

    m_controls = new QGraphicsWidget(this);
    m_controls->setLayout(m_header);
    m_controls->setZValue(99);
    m_controls->resize(100, 70);
    m_controls->hide();
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
    
    if(m_menu) {
        delete m_menu;
    }
}

void PreviewContainment::setupHeader()
{
    // setup toolboxes
    form = new Plasma::IconWidget(this);

    refresh = new Plasma::IconWidget(this);

    location = new Plasma::IconWidget(this);
    
    wallpaper = new Plasma::IconWidget(this);

    // add actions
    KAction *action0 = new KAction(KIcon("list-add"), "", this);
    connect(action0, SIGNAL(triggered()), this, SLOT(changeWallpaper()));
    wallpaper->setAction(action0);

    KAction *action1 = new KAction(KIcon("list-add"), "", this);
    connect(action1, SIGNAL(triggered()), this, SLOT(changeFormFactor()));
    form->setAction(action1);

    KAction *action2 = new KAction(KIcon("list-add"), "", this);
    connect(action2, SIGNAL(triggered()), this, SLOT(changeLocation()));
    location->setAction(action2);

    KAction *action3 = new KAction(KIcon("user-desktop"), "", this);
    connect(action3, SIGNAL(triggered()), this, SLOT(refreshApplet()));
    refresh->setAction(action3);

    // add the toolboxes
    m_header->addItem(wallpaper);
    m_header->addItem(refresh);
    m_header->addItem(form);
    m_header->addItem(location);
}

// Fixme(?): Does not currently respect arguments passed in during
// initialization. Will reload applet without arguments
// reloaded applet is also often smaller than original for some reason
void PreviewContainment::refreshApplet()
{
    if (!m_applet)
      return;
    clearApplets();

    if (m_applet->package()) {
      m_applet = Applet::loadPlasmoid(m_applet->package()->path());
      addApplet(m_applet, QPointF(-1, -1), false);
    } else {
      m_applet = addApplet(m_applet->pluginName(), 
                            QVariantList(), QRectF(0, 0, -1, -1));
    }

    m_applet->setFlag(QGraphicsItem::ItemIsMovable, false);
}

void PreviewContainment::changeFormFactor()
{
    initMenu();

    QAction *planar = addMenuItem(KIcon("user-desktop"),
                                  i18n("Planar"));
    QAction *mediacenter = addMenuItem(KIcon("user-desktop"),
                                       i18n("Media Center"));
    QAction *horizontal = addMenuItem(KIcon("user-desktop"),
                                      i18n("Horizontal"));
    QAction *vertical = addMenuItem(KIcon("user-desktop"),
                                    i18n("Vertical"));
    QAction *cancel = addMenuItem(KIcon("dialog-cancel"),
                                  i18n("Cancel"));

    connect(planar, SIGNAL(triggered(bool)), this, SLOT(planarTriggered(bool)));
    connect(mediacenter, SIGNAL(triggered(bool)), this, SLOT(mediacenterTriggered(bool)));
    connect(horizontal, SIGNAL(triggered(bool)), this, SLOT(horizontalTriggered(bool)));
    connect(vertical, SIGNAL(triggered(bool)), this, SLOT(verticalTriggered(bool)));
    connect(cancel, SIGNAL(triggered(bool)), this, SLOT(cancelOption(bool)));

    showMenu();
}

void PreviewContainment::changeWallpaper()
{
    initMenu();

    QSignalMapper *signalMapper = new QSignalMapper(this);

    // create a menu item for each wallpaper plugin installed
    foreach (const KPluginInfo &info, Plasma::Wallpaper::listWallpaperInfo()) {
      QAction *plugin = addMenuItem(KIcon("user-desktop"),
                                           info.pluginName());
      signalMapper->setMapping(plugin, info.pluginName());
      connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(setWallpaperPlugin(const QString &)));
      connect(plugin, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    }
    
    // cancel button
    QAction *cancel = addMenuItem(KIcon("dialog-cancel"), i18n("Cancel"));
    connect(cancel, SIGNAL(triggered(bool)), this, SLOT(cancelOption(bool)));

    showMenu();
}

void PreviewContainment::planarTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setFormFactor(Plasma::Planar);
}

void PreviewContainment::mediacenterTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::horizontalTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::verticalTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setFormFactor(Plasma::Vertical);
}

void PreviewContainment::changeLocation()
{
    initMenu();

    QAction *floating = addMenuItem(KIcon("user-trash"),
                                    i18n("Floating"));
    QAction *desktop = addMenuItem(KIcon("user-trash"),
                                   i18n("Desktop"));
    QAction *fullscreen = addMenuItem(KIcon("user-trash"),
                                      i18n("FullScreen"));
    QAction *topEdge = addMenuItem(KIcon("user-trash"),
                                   i18n("Top Edge"));
    QAction *bottomEdge = addMenuItem(KIcon("user-trash"),
                                      i18n("Bottom Edge"));
    QAction *leftEdge = addMenuItem(KIcon("user-trash"),
                                    i18n("Left Edge"));
    QAction *rightEdge = addMenuItem(KIcon("user-trash"),
                                     i18n("Right Edge"));
    QAction *cancel = addMenuItem(KIcon("dialog-cancel"),
                                  i18n("Cancel"));

    connect(floating, SIGNAL(triggered(bool)), this, SLOT(floatingTriggered(bool)));
    connect(desktop, SIGNAL(triggered(bool)), this, SLOT(desktopTriggered(bool)));
    connect(fullscreen, SIGNAL(triggered(bool)), this, SLOT(fullscreenTriggered(bool)));
    connect(topEdge, SIGNAL(triggered(bool)), this, SLOT(topEdgeTriggered(bool)));
    connect(bottomEdge, SIGNAL(triggered(bool)), this, SLOT(bottomEdgeTriggered(bool)));
    connect(leftEdge, SIGNAL(triggered(bool)), this, SLOT(leftEdgeTriggered(bool)));
    connect(rightEdge, SIGNAL(triggered(bool)), this, SLOT(rightEdgeTriggered(bool)));
    connect(cancel, SIGNAL(triggered(bool)), this, SLOT(cancelOption(bool)));

    showMenu();
}

void PreviewContainment::floatingTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::Desktop);
}

void PreviewContainment::desktopTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::Desktop);
}

void PreviewContainment::fullscreenTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::FullScreen);
}

void PreviewContainment::topEdgeTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::TopEdge);
}

void PreviewContainment::bottomEdgeTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::BottomEdge);
}

void PreviewContainment::leftEdgeTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::LeftEdge);
}

void PreviewContainment::rightEdgeTriggered(bool)
{
    if (m_options)
        m_options->hide();
    setLocation(Plasma::RightEdge);
}

void PreviewContainment::cancelOption(bool)
{
    if (m_options)
        m_options->hide();
}

void PreviewContainment::setWallpaperPlugin(const QString& name) {
    if (m_options)
        m_options->hide();
    setWallpaper(name);
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
    m_applet = applet;
    m_layout->removeItem(m_tmp);

    m_layout->addItem(applet);
    m_layout->setStretchFactor(applet, 3);
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

void PreviewContainment::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    m_controls->show();
    Containment::hoverEnterEvent(event);
}

void PreviewContainment::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    m_controls->hide();
    Containment::hoverLeaveEvent(event);
}

void PreviewContainment::initMenu() {
    if ((size().height() < MIN_HEIGHT_FOR_OVERLAY) || (size().width() < MIN_WIDTH_FOR_OVERLAY)) {
        delete m_menu;
        m_menu = new KMenu();
        m_menutype = KMENU;
    } else {
        delete m_options;
        m_options = new OverlayToolBox(this);
        m_options->setGeometry(geometry());
        m_options->setZValue(100);
        m_menutype = OVERLAY;
    }
}

QAction *PreviewContainment::addMenuItem(const KIcon &icon, const QString& title) {
    if (m_menutype == OVERLAY) {
        KAction *ret = new KAction(icon, title, m_options);
        m_options->addTool(ret);
        return ret;
    }
    return m_menu->addAction(icon, title);
}

void PreviewContainment::showMenu() {
    if (m_menutype == KMENU)
        m_menu->popup(QCursor::pos());
    else
        m_options->show();
}

K_EXPORT_PLASMA_APPLET(studiopreviewer, PreviewContainment)

