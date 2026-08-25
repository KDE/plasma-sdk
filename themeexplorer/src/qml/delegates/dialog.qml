/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.plasma.components as PlasmaComponents
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami

Item {
    id: root

    property string imagePath
    property bool showMargins

    KSvg.Svg {
        imagePath: root.imagePath
        onRepaintNeeded: {
            background.visible = background.hasElementPrefix("shadow")
        }
    }
    KSvg.FrameSvgItem {
        id: background
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        imagePath: root.imagePath
        onImagePathChanged: visible = hasElementPrefix("shadow")
        prefix: "shadow"
    }

    KSvg.FrameSvgItem {
        anchors {
            fill: background
            leftMargin: background.margins.left
            topMargin: background.margins.top
            rightMargin: background.margins.right
            bottomMargin: background.margins.bottom
        }
        imagePath: root.imagePath

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

    PlasmaComponents.Label {
        anchors.fill: parent
        text: root.imagePath
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
