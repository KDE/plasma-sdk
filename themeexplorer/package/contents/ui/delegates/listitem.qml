/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
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
