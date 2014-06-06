/*
 *  Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef VIEW_H
#define VIEW_H

#include "privateheaders/view.h"

class ViewerCorona;

class View : public PlasmaQuick::View
{
    Q_OBJECT
    Q_PROPERTY(bool konsoleVisible READ konsoleVisible CONSTANT);

public:
    View(ViewerCorona *corona, bool konsoleVisible, QWindow *parent = 0);
    ~View();

    void addApplet(const QString &applet);
    void addContainment(const QString &containment);
    void addFormFactor(const QString &formFactor = QStringLiteral("planar"));
    void addLocation(const QString &location = QStringLiteral("floating"));
    bool konsoleVisible();

    Q_INVOKABLE void changeFormFactor(int formFactor);
    Q_INVOKABLE void changeLocation(int location);
    Q_INVOKABLE void takeScreenShot();

    static ViewerCorona *createCorona();

protected:
    QString pluginFromPath(const QString &path) const;

private:
    QString m_lastAppletName;
    bool m_konsoleVisible;
};

#endif // VIEW_H
