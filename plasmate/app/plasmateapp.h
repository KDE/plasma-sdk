/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the
    Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLASMAAPP_H
#define PLASMAAPP_H

#include <QObject>
#include <QHash>

class QAction;

class PlasmateApp : public QObject {

    Q_OBJECT

public:
    PlasmateApp(QObject *parent = 0);
    ~PlasmateApp();

private Q_SLOTS:
    void loadMainWindow(const QUrl &projectFile);
    void checkStartPage();

private:
    void init();
    void setupStartPage(bool closeProjectsAndDocuments);

    void showKDevUi(bool visible);
    QList<QAction*> m_toolbarActions;
};

#endif


