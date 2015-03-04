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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    Item {
        id: background
        anchors {
            fill: parent
            margins: units.gridUnit
        }
        clip: true

        PlasmaCore.Svg {
            id: lineSvg
            imagePath: "widgets/listitem"
        }
        Column {
            anchors {
                fill: parent
                bottomMargin: label.top
            }
            Repeater {
                model: 5
                PlasmaCore.FrameSvgItem {
                    width: background.width
                    height: units.gridUnit * 3
                    imagePath: "widgets/listitem"
                    prefix: modelData == 0 ? "section" : (modelData == 1 ? "pressed" : "normal")

                    Rectangle {
                        id: marginsRectangle
                        anchors {
                            fill: parent
                            leftMargin: parent.margins.left
                            topMargin: parent.margins.top
                            rightMargin: parent.margins.right
                            bottomMargin: parent.margins.bottom
                        }
                        color: "green"
                        opacity: 0.5
                        visible: root.showMargins
                    }
                    PlasmaComponents.Label {
                        anchors.centerIn: parent
                        text: parent.prefix
                    }
                    PlasmaCore.SvgItem {
                        svg: lineSvg
                        elementId: "separator"
                        height: naturalSize.height
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.bottom
                        }
                    }
                }
            }
        }
    }

    PlasmaComponents.Label {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        text: model.imagePath
        visible: width < marginsRectangle.width
    }
}
