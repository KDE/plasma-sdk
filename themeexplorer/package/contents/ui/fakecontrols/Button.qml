/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.15

Item {
    implicitWidth: parent.width/1.2
    implicitHeight: units.gridUnit * 1.6

    Rectangle {
        id: button
        anchors.fill: parent
        radius: units.smallSpacing/2
        color: buttonMouse.pressed ? Qt.darker(buttonBackgroundColor, 1.5) : buttonBackgroundColor
        Label {
            anchors.centerIn: parent
            text: i18n("Button")
            color: buttonTextColor
        }
        MouseArea {
            id: buttonMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: button.focus = true
        }
        Rectangle {
            anchors.fill: parent
            radius: units.smallSpacing/2
            visible: buttonMouse.containsMouse || button.focus
            color: "transparent"
            border {
                color: buttonMouse.containsMouse ? buttonHoverColor : buttonFocusColor
            }
        }
    }
    DropShadow {
        anchors.fill: button
        horizontalOffset: 0
        verticalOffset: units.smallSpacing/4
        radius: units.smallSpacing 
        samples: 16
        color: buttonMouse.pressed ? Qt.rgba(0, 0, 0, 0) : Qt.rgba(0, 0, 0, 0.5)
        source: button
    }
}
