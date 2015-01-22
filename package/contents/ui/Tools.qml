/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaComponents.ToolBar {
    /*
    Rectangle {

        color: theme.backgroundColor

        Rectangle {
            color: theme.highlightColor
            height: units.gridUnit / 20
            y: preview.y
            width: preview.x
            anchors {
                left: parent.left
            }
        }
    }
    */
        Rectangle {
            color: theme.backgroundColor
            height: units.gridUnit / 20
            width: preview.width
            anchors {
                right: parent.right
                top: parent.bottom
                //right: parent.right
            }
        }
    tools: RowLayout {

        spacing: units.gridUnit / 2

        PlasmaComponents.TextField {
            id: filterInput

            Layout.fillWidth: true

            onTextChanged: typingtimer.restart()

            Timer {
                id: typingtimer
                running: false
                repeat: false
                interval: 100
                onTriggered: {
                    iconModel.filter = filterInput.text
                }
            }
        }

        ComboBox {
            width: 200
            model: iconModel.categories
            onCurrentIndexChanged: {
                if (currentText != "") {
                    iconModel.category = currentText
                }
            }
        }

        ComboBox {
            width: 200
            model: iconModel.plasmathemes
            onCurrentIndexChanged: {
                if (currentText != "") {
                    iconModel.plasmaTheme = currentText
                }
            }
        }

        CheckBox {
            text: "Enable Hover"
            onCheckedChanged: hoveredHighlight = checked
        }

        Slider {
            id: sizeslider
            //anchors.verticalCenter: parent.verticalCenter
            Layout.preferredWidth: preview.width - units.gridUnit * 2

            maximumValue: 6.0
            stepSize: 1.0
            tickmarksEnabled: true
            value: 4

            onValueChanged: sizetimer.restart()

            Timer {
                id: sizetimer
                running: false
                repeat: false
                interval: 200
                onTriggered: {
                    print("Size is now: " + sizeslider.value + " => " + indexToSize(sizeslider.value));
                    iconSize = indexToSize(sizeslider.value);
                }
            }
        }

        PlasmaComponents.Label {
            anchors.verticalCenter: parent.verticalCenter
            text: iconSize
        }
    }
}

