/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

Item {
    id: root

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

    Item {
        anchors.fill: backgroundRect
        clip: true
        Column {
            anchors.centerIn: parent
            PlasmaComponents.ToolButton {
                flat: false
                icon.source: "window-close"
            }
            PlasmaComponents.RadioButton {
                text: i18n("Option")
                checked: true
            }
            PlasmaComponents.RadioButton {
                text: i18n("Option")
                checked: false
            }
            PlasmaComponents.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.imagePath
                visible: width < backgroundRect.width
            }
        }
}
}
