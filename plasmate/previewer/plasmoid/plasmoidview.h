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

#ifndef PLASMOIDVIEW_H
#define PLASMOIDVIEW_H

#include "previewcontainment.h"

#include <QGraphicsView>

#include <Plasma/Corona>

using namespace Plasma;

class PlasmoidView : public QGraphicsView
{
    Q_OBJECT

public:
    PlasmoidView(QWidget *parent = 0);
    ~PlasmoidView();

    void addApplet(const QString &name, const QString &containment =  QString(), const QVariantList &args = QVariantList());
    void clearApplets();

private slots:
    void sceneRectChanged(const QRectF &rect);
    void resizeEvent(QResizeEvent *event);
    void appletRemoved(Plasma::Applet *applet);

private:
    bool hasStorageGroup() const;
    KConfigGroup storageGroup() const;
    bool hasContainmentStorageGroup() const;
    KConfigGroup containmentStorageGroup() const;

    Plasma::Corona m_corona;
    Plasma::FormFactor m_formfactor;
    Plasma::Containment *m_containment;
    Plasma::Applet *m_applet;

    QString m_currentPath;
};

#endif //PLASMOIDVIEW_H
