/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents3

Item {
    Item {
        id: background
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        clip: true

        PlasmaCore.Svg {
            id: lineSvg
            imagePath: "widgets/listitem"
        }
        Column {
            anchors {
                fill: parent
            }
            Repeater {
                model: 5
                PlasmaCore.FrameSvgItem {
                    width: parent.width
                    height: Kirigami.Units.gridUnit * 3
                    imagePath: "widgets/listitem"
                    prefix: modelData === 0 ? "section" : (modelData === 1 ? "pressed" : "normal")

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
                    PlasmaComponents3.Label {
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

        PlasmaComponents3.Label {
            id: label
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }
            text: model.imagePath
            visible: width < parent.width
        }
    }
}
