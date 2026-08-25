/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ksvg as KSvg
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

import org.kde.plasma.themeexplorer

Item {
    id: root

    property string imagePath
    property string showMargins

    Item {
        id: backgroundRect
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        clip: true

        Column {
            anchors.fill: parent
            Repeater {
                model: 5
                KSvg.FrameSvgItem {
                    required property int index
                    width: backgroundRect.width
                    height: Kirigami.Units.gridUnit * 3
                    imagePath: "widgets/listitem"
                    prefix: index === 0 ? "section" : (index === 1 ? "pressed" : "normal")

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
                    KSvg.SvgItem {
                        imagePath: "widgets/listitem"
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
        text: root.imagePath
        visible: width < backgroundRect.width
    }
}
