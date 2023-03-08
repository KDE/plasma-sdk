/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

Item {
    Rectangle {
        id: background
        anchors {
            fill: parent
            margins: units.gridUnit
        }
        radius: 3
        color: theme.backgroundColor
        opacity: 0.6
    }
    PlasmaCore.Svg {
        id: monitorSvg
        imagePath: "widgets/monitor"
    }
    PlasmaCore.SvgItem {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: screen.bottom
            topMargin: -units.gridUnit
        }
        svg: monitorSvg
        elementId: "base"
    }
    PlasmaCore.FrameSvgItem {
        id: screen
        anchors {
            left: background.left
            top: background.top
            right: background.right
            margins: units.gridUnit
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
            horizontalCenter: background.horizontalCenter
            bottom: background.bottom
        }
        text: model.imagePath
        visible: width < background.width
    }
}
