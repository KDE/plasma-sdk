// -*- coding: iso-8859-1 -*-
/*
 *   Copyright 2011-2013 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.draganddrop 2.0 as DragDrop

import "plasmapackage:/code/LayoutManager.js" as LayoutManager

Item {
    id: root
    property int iconSize: 16
    property var container

    Column {
        spacing:0
        anchors.centerIn: parent

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            PlasmaComponents.ToolButton {
                iconSource: "plasma"
            }
            PlasmaComponents.ToolButton {
                iconSource: "plasmagik"
            }
        }


        Item {
            id: appletContainer
            anchors.horizontalCenter: parent.horizontalCenter
            width: root.width/4
            height:root.height/4
        }

        onImplicitWidthChanged: {
            if (typeof container != 'undefined') {
                container.width = root.width/4;
            }
        }

        onImplicitHeightChanged: {
            if (typeof container != 'undefined') {
                container.height = root.height/4;
            }
        }

        Connections {
            target: plasmoid

            onAppletAdded: {
                var component = Qt.createComponent("AppletAppearance.qml");
                var e = component.errorString();
                if (e != "") {
                    print("Error loading AppletAppearance.qml: " + component.errorString());
                }

                container = component.createObject(appletContainer)
                console.log(appletContainer.x)
                console.log(appletContainer.y)
                applet.parent = container;
                applet.visible = true;
                container.applet = applet;
                container.width = appletContainer.width;
                container.height = appletContainer.height;
            }

            onAppletRemoved: {
            }
        }
    }
}
