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


#ifndef PREVIEWCONTAINMENT_H
#define PREVIEWCONTAINMENT_H

#include "overlaytoolbox.h"

#include <QGraphicsView>
#include <QGraphicsLinearLayout>
#include <KMenu>

#include <Plasma/Containment>
#include <Plasma/IconWidget>


class PreviewContainment : public Plasma::Containment
{
    Q_OBJECT

public:
    PreviewContainment(QObject *parent, const QVariantList &args = QVariantList());
    ~PreviewContainment();

public slots:
    void changeFormFactor();
    void changeLocation();
    void changeWallpaper();
    void refreshApplet();

protected slots:
    void constraintsEvent(Plasma::Constraints constraints);

    void onAppletAdded(Plasma::Applet *, const QPointF &);
    void onAppletRemoved(Plasma::Applet *);
    void onAppletGeometryChanged();

    void cancelOption(bool);

    void planarTriggered(bool);
    void mediacenterTriggered(bool);
    void horizontalTriggered(bool);
    void verticalTriggered(bool);

    void floatingTriggered(bool);
    void desktopTriggered(bool);
    void fullscreenTriggered(bool);
    void topEdgeTriggered(bool);
    void bottomEdgeTriggered(bool);
    void leftEdgeTriggered(bool);
    void rightEdgeTriggered(bool);

    void setWallpaperPlugin(const QString&);

protected:
    void setupHeader();
    void destroyHeader();
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    // these are a rough gauge for now
    static const int MIN_HEIGHT_FOR_OVERLAY = 300;
    static const int MIN_WIDTH_FOR_OVERLAY = 200;

    enum menu_type { KMENU, OVERLAY } m_menutype;
    OverlayToolBox *m_options;
    KMenu *m_menu;

    // Little dirty hack to make previewer
    // look more beautiful when there is no applet.
    // XXX: Improve this ?
    QGraphicsWidget *m_tmp;
    QGraphicsWidget *m_controls;

    QGraphicsLinearLayout *m_layout;
    QGraphicsLinearLayout *m_header;

    Plasma::Applet *m_applet;

    Plasma::IconWidget *form;
    Plasma::IconWidget *refresh;
    Plasma::IconWidget *location;
    Plasma::IconWidget *wallpaper;

    void initMenu(const QString &title);
    QAction* addMenuItem(const KIcon &icon, const QString &title);
    void showMenu();
};


#endif //PREVIEWCONTAINMENT
