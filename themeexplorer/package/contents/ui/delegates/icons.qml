/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    id: iconsDelegate
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
        id: iconsSvg
        imagePath: model.imagePath
    }
    property var iconElements: model.iconElements

    Flow {
        clip: true
        anchors {
            fill: background
            margins: units.gridUnit
        }
        Repeater {
            model: iconsDelegate.iconElements
            delegate: PlasmaCore.SvgItem {
                svg: iconsSvg
                elementId: modelData
                width: naturalSize.width
                height: naturalSize.height
            }
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
