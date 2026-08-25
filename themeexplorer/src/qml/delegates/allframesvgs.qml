/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.components as PlasmaComponents
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

Item {
    id: root

    property string imagePath
    property string frameSvgPrefixes
    property string showMargins

    Rectangle {
        id: backgroundRect
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        radius: 3
        color: Kirigami.Theme.backgroundColor
        opacity: 0.6
    }
    Flow {
        id: flow
        clip: true
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit * 2
        }
        Repeater {
            model: root.frameSvgPrefixes
            delegate: KSvg.FrameSvgItem {
                id: svgItem
                required property string modelData
                width: flow.width / 2
                height: flow.height / 3
                imagePath: root.imagePath
                prefix: modelData
                PlasmaComponents.Label {
                    anchors.centerIn: parent
                    text: svgItem.modelData
                    visible: width < parent.width
                }

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
            }
        }
    }

    PlasmaComponents.Label {
        anchors {
            horizontalCenter: backgroundRect.horizontalCenter
            bottom: backgroundRect.bottom
        }
        text: root.imagePath
        visible: width < flow.width
    }
}
