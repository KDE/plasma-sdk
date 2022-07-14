/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1

MouseArea {
    id: buttonMouse
    hoverEnabled: true
    implicitWidth: parent.width/1.2
    implicitHeight: layout.height 

    property bool checked: true
    //in real controls this is done by the color scope
    property bool complementary: false

    onClicked: {
        buttonMouse.focus = true
        checked = !checked
    }
    RowLayout {
        id: layout
        height: units.gridUnit * 1.6
        Rectangle {
            id: button
            radius: units.smallSpacing/2
            color: buttonMouse.pressed ? Qt.darker(buttonBackgroundColor, 1.5) : buttonBackgroundColor
            height: parent.height / 1.2
            width: height
            Rectangle {
                anchors.fill: parent
                radius: units.smallSpacing/2
                visible: buttonMouse.containsMouse || buttonMouse.focus
                color: "transparent"
                border {
                    color: {
                        if (buttonMouse.checked) {
                            return buttonMouse.complementary ? complementaryHoverColor : highlightColor;
                        } else if (buttonMouse.containsMouse) {
                            return buttonHoverColor;
                        } else {
                           return buttonFocusColor;
                        }
                    }
                }
            }
            Rectangle {
                anchors {
                    fill: parent
                    margins: units.smallSpacing
                }
                radius: units.smallSpacing/2
                visible: buttonMouse.checked
                color: buttonMouse.complementary ? complementaryHoverColor : highlightColor
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
        Label {
            anchors.centerIn: parent
            text: i18n("Checkbox")
            color: buttonMouse.complementary ? complementaryTextColor : textColor
        }
    }
}
