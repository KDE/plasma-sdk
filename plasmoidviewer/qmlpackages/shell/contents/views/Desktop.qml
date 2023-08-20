/*
 *  SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
//import org.kde.plasma 2.0

import org.kde.kirigami 2.20 as Kirigami


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

        onFormFactor: FormFactorType => {
            desktop.changeFormFactor(FormFactorType);
        }
        onLocation: LocationType => {
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
