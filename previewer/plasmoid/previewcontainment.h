/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007 Aaron Seigo <aseigo@kde.org>
 * Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 * Copyright 2009 Artur Duque de Souza <morpheuz@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef PREVIEWCONTAINMENT_H
#define PREVIEWCONTAINMENT_H

#include <QGraphicsView>
#include <QGraphicsLinearLayout>

#include <Plasma/Containment>

class KMenu;

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

signals:
    void refreshClicked();

protected slots:
    void refreshAndEmit();
    void constraintsEvent(Plasma::Constraints constraints);

    void onAppletAdded(Plasma::Applet *, const QPointF &);
    void onAppletRemoved(Plasma::Applet *);

    void formFactorSelected(QAction *);
    void locationSelected(QAction *);
    void wallpaperSelected(QAction *);

protected:
    void setupHeader();
    void destroyHeader();
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    Plasma::Applet *m_applet;
    KMenu *m_menu;

    // Little dirty hack to make previewer
    // look more beautiful when there is no applet.
    // XXX: Improve this ?
    QGraphicsWidget *m_tmp;
    QGraphicsWidget *m_controls;

    QGraphicsLinearLayout *m_layout;
    QGraphicsLinearLayout *m_header;

    void initMenu(const QString &title);
    void showMenu();
};


#endif //PREVIEWCONTAINMENT
