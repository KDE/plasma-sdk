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
    PlasmaCore.Svg {
        imagePath: model.imagePath
        onRepaintNeeded: {
            topPanelShadow.visible = topPanelShadow.hasElementPrefix("shadow")
            leftPanelShadow.visible = leftPanelShadow.hasElementPrefix("shadow")
            rightPanelShadow.visible = rightPanelShadow.hasElementPrefix("shadow")
            bottomPanelShadow.visible = bottomPanelShadow.hasElementPrefix("shadow")
        }
    }
    Rectangle {
        id: background
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        radius: 3
        color: theme.backgroundColor
        opacity: 0.6
    }

    PlasmaCore.FrameSvgItem {
        id: bottomPanelShadow
        anchors {
            horizontalCenter: background.horizontalCenter
            bottom: background.bottom
        }
        width: background.width * 0.7
        height: background.width * 0.2
        enabledBorders: PlasmaCore.FrameSvgItem.TopBorder | PlasmaCore.FrameSvgItem.LeftBorder | PlasmaCore.FrameSvgItem.RightBorder
        imagePath: model.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    PlasmaCore.FrameSvgItem {
        anchors {
            fill: bottomPanelShadow
            topMargin: bottomPanelShadow.margins.top
            leftMargin: bottomPanelShadow.margins.left
            rightMargin: bottomPanelShadow.margins.right
            bottomMargin: bottomPanelShadow.margins.bottom
        }
        imagePath: model.imagePath
        enabledBorders: bottomPanelShadow.enabledBorders

        Rectangle {
            id: marginsRectangle
            anchors {
                fill: parent
                topMargin: parent.margins.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }
            color: "green"
            opacity: 0.5
            visible: root.showMargins
        }
    }

    PlasmaCore.FrameSvgItem {
        id: leftPanelShadow
        anchors {
            verticalCenter: background.verticalCenter
            left: background.left
        }
        height: background.height - background.width * 0.2
        width: background.width * 0.2
        enabledBorders: PlasmaCore.FrameSvgItem.TopBorder | PlasmaCore.FrameSvgItem.BottomBorder | PlasmaCore.FrameSvgItem.RightBorder
        imagePath: model.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    PlasmaCore.FrameSvgItem {
        anchors {
            fill: leftPanelShadow
            topMargin: leftPanelShadow.margins.top
            leftMargin: leftPanelShadow.margins.left
            rightMargin: leftPanelShadow.margins.right
            bottomMargin: leftPanelShadow.margins.bottom
        }
        imagePath: model.imagePath
        enabledBorders: leftPanelShadow.enabledBorders

        Rectangle {
            anchors {
                fill: parent
                topMargin: parent.margins.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }
            color: "green"
            opacity: 0.5
            visible: root.showMargins
        }
    }

    PlasmaCore.FrameSvgItem {
        id: topPanelShadow
        anchors {
            horizontalCenter: background.horizontalCenter
            top: background.top
        }
        width: background.width * 0.7
        height: background.width * 0.2
        enabledBorders: PlasmaCore.FrameSvgItem.BottomBorder | PlasmaCore.FrameSvgItem.LeftBorder | PlasmaCore.FrameSvgItem.RightBorder
        imagePath: model.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    PlasmaCore.FrameSvgItem {
        anchors {
            fill: topPanelShadow
            topMargin: topPanelShadow.margins.top
            leftMargin: topPanelShadow.margins.left
            rightMargin: topPanelShadow.margins.right
            bottomMargin: topPanelShadow.margins.bottom
        }
        imagePath: model.imagePath
        enabledBorders: topPanelShadow.enabledBorders

        Rectangle {
            anchors {
                fill: parent
                topMargin: parent.margins.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }
            color: "green"
            opacity: 0.5
            visible: root.showMargins
        }
    }

    PlasmaCore.FrameSvgItem {
        id: rightPanelShadow
        anchors {
            verticalCenter: background.verticalCenter
            right: background.right
        }
        height: background.height - background.width * 0.2
        width: background.width * 0.2
        enabledBorders: PlasmaCore.FrameSvgItem.TopBorder | PlasmaCore.FrameSvgItem.BottomBorder | PlasmaCore.FrameSvgItem.LeftBorder
        imagePath: model.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    PlasmaCore.FrameSvgItem {
        anchors {
            fill: rightPanelShadow
            topMargin: rightPanelShadow.margins.top
            leftMargin: rightPanelShadow.margins.left
            rightMargin: rightPanelShadow.margins.right
            bottomMargin: rightPanelShadow.margins.bottom
        }
        imagePath: model.imagePath
        enabledBorders: rightPanelShadow.enabledBorders

        Rectangle {
            anchors {
                fill: parent
                topMargin: parent.margins.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }
            color: "green"
            opacity: 0.5
            visible: root.showMargins
        }
    }

    PlasmaComponents3.Label {
        anchors.centerIn: parent
        text: model.imagePath
        visible: width < marginsRectangle.width
    }
}
