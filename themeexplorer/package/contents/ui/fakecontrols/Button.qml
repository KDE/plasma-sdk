/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
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

import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2

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
