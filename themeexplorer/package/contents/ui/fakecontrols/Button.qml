/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import Qt5Compat.GraphicalEffects
import QtQuick.Controls 2.15
import org.kde.kirigami as Kirigami

Item {
    implicitWidth: parent.width/1.2
    implicitHeight: Kirigami.Units.gridUnit * 1.6

    Rectangle {
        id: button
        anchors.fill: parent
        radius: Kirigami.Units.smallSpacing/2
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
            radius: Kirigami.Units.smallSpacing/2
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
        verticalOffset: Kirigami.Units.smallSpacing/4
        radius: Kirigami.Units.smallSpacing
        samples: 16
        color: buttonMouse.pressed ? Qt.rgba(0, 0, 0, 0) : Qt.rgba(0, 0, 0, 0.5)
        source: button
    }
}
