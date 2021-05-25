/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2

Item {
    implicitWidth: parent.width/1.2
    implicitHeight: units.gridUnit * 1.6

    Rectangle {
        id: view
        anchors.fill: parent
        radius: units.smallSpacing/2
        color: viewBackgroundColor
        MouseArea {
            id: viewMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: input.focus = true
        }
        TextInput {
            id: input
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: units.smallSpacing
            }
            text: i18n("Text input…")
            color: viewTextColor
        }
        Rectangle {
            anchors.fill: parent
            radius: units.smallSpacing/2
            visible: viewMouse.containsMouse || input.focus
            color: "transparent"
            border {
                color: viewMouse.containsMouse ? viewHoverColor : viewFocusColor
            }
        }
    }
    InnerShadow {
        anchors.fill: view
        horizontalOffset: 0
        verticalOffset: units.smallSpacing/4
        radius: units.smallSpacing 
        samples: 16
        color: viewMouse.pressed ? Qt.rgba(0, 0, 0, 0) : Qt.rgba(0, 0, 0, 0.5)
        source: view
    }
}
