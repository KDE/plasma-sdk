/*
 *  Copyright 2012 Marco Martin <mart@kde.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
//import org.kde.plasma 2.0

import org.kde.plasma.core 2.0 as PlasmaCore


Rectangle {
    id: root
    color: "black"
    width: 640
    height: 480

    property Item containment

    SdkButtons {
        id: buttons
        z: +1
        anchors {
            fill: backgroundButtons
            leftMargin: backgroundButtons.margins.left
            rightMargin: backgroundButtons.margins.right - 8
            topMargin: backgroundButtons.margins.top
            bottomMargin: backgroundButtons.margins.bottom
        }

        onFormFactor: {
            desktop.changeFormFactor(FormFactorType)
        }
        onLocation: {
            desktop.changeLocation(LocationType)
        }
    }

    Background {
        id: backgroundButtons
        width: buttons.backgroundWidth
        height: buttons.backgroundHeight
        anchors.horizontalCenter: root.horizontalCenter
    }

    Konsole {
        id: konsolePreviewer
        z: +1
        anchors {
            fill: backgroundKonsole
            leftMargin: backgroundKonsole.margins.left + 8
            rightMargin: backgroundKonsole.margins.right + 8
            topMargin: backgroundKonsole.margins.top
            bottomMargin: backgroundKonsole.margins.bottom + 8
        }
        width: backgroundKonsole.width/2
        height: backgroundKonsole.height/4
    }

    Background {
        id: backgroundKonsole
        width: root.width/2
        height: root.height/3
        anchors.horizontalCenter: root.horizontalCenter
        anchors.bottom: root.bottom
    }


    onContainmentChanged: {
        print("New Containment: " + containment)
        //containment.parent = root
        containment.visible = true
        containment.anchors.top = buttons.bottom
    }

    Component.onCompleted: {
        desktop.stayBehind = true;
        desktop.fillScreen = true;
        print("View QML loaded")
    }
}
