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

#include <QGraphicsView>
#include <QGraphicsLinearLayout>

#include <Plasma/Containment>
#include <Plasma/Label>


class PreviewContainment : public Plasma::Containment
{
    Q_OBJECT

public:
    PreviewContainment(QObject *parent, const QVariantList &args = QVariantList());

protected slots:
    void onAppletAdded(Plasma::Applet *, const QPointF &);
    void onAppletRemoved(Plasma::Applet *);
    void onAppletGeometryChanged();

private:
    QGraphicsLinearLayout *m_layout;
    Plasma::Label *label;
};


#endif //PREVIEWCONTAINMENT
