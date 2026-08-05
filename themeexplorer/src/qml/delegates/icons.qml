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

    property string iconElements //TODO add
    property string imagePath

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
        clip: true
        anchors {
            fill: backgroundRect
            margins: Kirigami.Units.gridUnit
        }
        Repeater {
            model: root.iconElements
            delegate: KSvg.SvgItem {
                required property string modelData
                imagePath: root.imagePath
                elementId: modelData
                width: naturalSize.width
                height: naturalSize.height
            }
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
