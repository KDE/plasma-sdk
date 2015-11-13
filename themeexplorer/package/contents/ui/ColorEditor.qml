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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

Dialog {
    id: dialog
    visible: true
    property alias textColor: textColorButton.color
    property alias backgroundColor: backgroundButton.color

    property alias buttonTextColor: buttonTextButton.color
    property alias buttonBackgroundColor: buttonBackgroundButton.color
    property alias buttonHoverColor: buttonHoverButton.color
    property alias buttonFocusColor: buttonFocusButton.color

    title: i18n("Edit Colors");
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    ColorDialog {
        id: colorDialog
        modality: Qt.WindowModal
        showAlphaChannel: false
        title: i18n("Select Color")
        property Item activeButton
        onAccepted: {
            activeButton.color = color;
        }
    }
    ColumnLayout {
        anchors {
            left: parent.left
            right: parent.right
        }
        Label {
            id: errorMessage
            Layout.fillWidth: true
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: units.gridUnit * 10
            Rectangle {
                id: plasmoidPreview
                anchors.centerIn: parent
                width: parent.width/1.5
                height: parent.height/1.5
                radius: units.smallSpacing
                color: backgroundColor
                ColumnLayout {
                    anchors.centerIn: parent
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: i18n("Normal text")
                        color: textColor
                    }
                    Rectangle {
                        id: button
                        Layout.alignment: Qt.AlignHCenter
                        implicitWidth: parent.width/1.2
                        implicitHeight: units.gridUnit * 1.6
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
            }
            DropShadow {
                anchors.fill: plasmoidPreview
                horizontalOffset: 0
                verticalOffset: units.smallSpacing/2
                radius: units.gridUnit / 2.2
                samples: 16
                color: Qt.rgba(0, 0, 0, 0.5)
                source: plasmoidPreview
            }
        }
        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            columnSpacing: units.smallSpacing

            
            FormLabel {
                text: i18n("Text color:")
                buddy: textColorButton
            }
            ColorButton {
                id: textColorButton
            }
            FormLabel {
                text: i18n("Background color:")
                buddy: backgroundButton
            }
            ColorButton {
                id: backgroundButton
            }

            FormLabel {
                text: i18n("Button text color:")
                buddy: buttonTextButton
            }
            ColorButton {
                id: buttonTextButton
            }
            FormLabel {
                text: i18n("Button background color:")
                buddy: buttonBackgroundButton
            }
            ColorButton {
                id: buttonBackgroundButton
            }
            FormLabel {
                text: i18n("Button mouse over color:")
                buddy: buttonHoverButton
            }
            ColorButton {
                id: buttonHoverButton
            }
            FormLabel {
                text: i18n("Button focus color:")
                buddy: buttonFocusButton
            }
            ColorButton {
                id: buttonFocusButton
            }
        }
    }

    onAccepted: {
        
    }
}
