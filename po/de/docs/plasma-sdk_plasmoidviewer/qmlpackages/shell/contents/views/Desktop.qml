/*
 *  SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
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
            desktop.changeFormFactor(FormFactorType);
        }
        onLocation: {
            desktop.changeLocation(LocationType);
        }
        onRequestScreenshot: {
            desktop.takeScreenShot();
        }
    }

    Background {
        id: backgroundButtons
        width: buttons.backgroundWidth
        height: buttons.backgroundHeight
        anchors {
            horizontalCenter: root.horizontalCenter
            bottom: root.bottom
        }
    }

    Konsole {
        id: konsolePreviewer
        z: +1
        visible: desktop.konsoleVisible
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
        visible: konsolePreviewer.visible
        width: root.width/2
        height: root.height/3
        anchors.horizontalCenter: root.horizontalCenter
        anchors.bottom: buttons.top
        anchors.bottomMargin: PlasmaCore.Units.smallSpacing
    }

    onContainmentChanged: {
        print("New Containment: " + containment)
        //containment.parent = root
        containment.visible = true
        containment.anchors.fill = root
    }

    Component.onCompleted: {
        desktop.stayBehind = true;
        desktop.fillScreen = true;
        print("View QML loaded")
    }
}
