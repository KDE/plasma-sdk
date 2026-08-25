/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.ksvg as KSvg
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

import org.kde.plasma.themeexplorer

Item {
    id: root

    property string imagePath
    property bool showMargins

    KSvg.Svg {
        imagePath: root.imagePath
        onRepaintNeeded: {
            bottomPanelShadow.visible = bottomPanelShadow.hasElementPrefix("shadow")
            topPanelShadow.visible = topPanelShadow.hasElementPrefix("shadow")
            leftPanelShadow.visible = leftPanelShadow.hasElementPrefix("shadow")
            rightPanelShadow.visible = rightPanelShadow.hasElementPrefix("shadow")
        }
    }
    Rectangle {
        id: background
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        radius: 3
        color: Kirigami.Theme.backgroundColor
        opacity: 0.6
    }

    KSvg.FrameSvgItem {
        id: bottomPanelShadow
        anchors {
            horizontalCenter: background.horizontalCenter
            bottom: background.bottom
        }
        width: background.width * 0.7
        height: background.width * 0.2
        enabledBorders: KSvg.FrameSvgItem.TopBorder | KSvg.FrameSvgItem.LeftBorder | KSvg.FrameSvgItem.RightBorder
        imagePath: root.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    KSvg.FrameSvgItem {
        anchors {
            fill: bottomPanelShadow
            leftMargin: bottomPanelShadow.margins.left
            topMargin: bottomPanelShadow.margins.top
            rightMargin: bottomPanelShadow.margins.right
            bottomMargin: bottomPanelShadow.margins.bottom
        }
        imagePath: root.imagePath
        enabledBorders: bottomPanelShadow.enabledBorders

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

    KSvg.FrameSvgItem {
        id: leftPanelShadow
        anchors {
            verticalCenter: background.verticalCenter
            left: background.left
        }
        height: background.height - background.width * 0.2
        width: background.width * 0.2
        enabledBorders: KSvg.FrameSvgItem.TopBorder | KSvg.FrameSvgItem.BottomBorder | KSvg.FrameSvgItem.RightBorder
        imagePath: root.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    KSvg.FrameSvgItem {
        anchors {
            fill: leftPanelShadow
            leftMargin: leftPanelShadow.margins.left
            topMargin: leftPanelShadow.margins.top
            rightMargin: leftPanelShadow.margins.right
            bottomMargin: leftPanelShadow.margins.bottom
        }
        imagePath: root.imagePath
        enabledBorders: leftPanelShadow.enabledBorders

        Rectangle {
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

    KSvg.FrameSvgItem {
        id: topPanelShadow
        anchors {
            horizontalCenter: background.horizontalCenter
            top: background.top
        }
        width: background.width * 0.7
        height: background.width * 0.2
        enabledBorders: KSvg.FrameSvgItem.BottomBorder | KSvg.FrameSvgItem.LeftBorder | KSvg.FrameSvgItem.RightBorder
        imagePath: root.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    KSvg.FrameSvgItem {
        anchors {
            fill: topPanelShadow
            leftMargin: topPanelShadow.margins.left
            topMargin: topPanelShadow.margins.top
            rightMargin: topPanelShadow.margins.right
            bottomMargin: topPanelShadow.margins.bottom
        }
        imagePath: root.imagePath
        enabledBorders: topPanelShadow.enabledBorders

        Rectangle {
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

    KSvg.FrameSvgItem {
        id: rightPanelShadow
        anchors {
            verticalCenter: background.verticalCenter
            right: background.right
        }
        height: background.height - background.width * 0.2
        width: background.width * 0.2
        enabledBorders: KSvg.FrameSvgItem.TopBorder | KSvg.FrameSvgItem.BottomBorder | KSvg.FrameSvgItem.LeftBorder
        imagePath: root.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    KSvg.FrameSvgItem {
        anchors {
            fill: rightPanelShadow
            leftMargin: rightPanelShadow.margins.left
            topMargin: rightPanelShadow.margins.top
            rightMargin: rightPanelShadow.margins.right
            bottomMargin: rightPanelShadow.margins.bottom
        }
        imagePath: root.imagePath
        enabledBorders: rightPanelShadow.enabledBorders

        Rectangle {
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

    PlasmaComponents.Label {
        anchors.fill: parent
        text: root.imagePath
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
