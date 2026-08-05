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
    KSvg.SvgItem {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: screen.bottom
            topMargin: -Kirigami.Units.gridUnit
        }
        imagePath: "widgets/monitor"
        elementId: "base"
    }
    KSvg.FrameSvgItem {
        id: screen
        anchors {
            left: backgroundRect.left
            top: backgroundRect.top
            right: backgroundRect.right
            margins: Kirigami.Units.gridUnit
        }
        height: width / 1.6
        imagePath: "widgets/monitor"

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
        anchors {
            horizontalCenter: backgroundRect.horizontalCenter
            bottom: backgroundRect.bottom
        }
        text: root.imagePath
        visible: width < backgroundRect.width
    }
}
