/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.components 3.0 as PlasmaComponents3

Item {
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

    Item {
        anchors.fill: background
        clip: true
        Column {
            anchors.centerIn: parent
            PlasmaComponents3.ToolButton {
                flat: false
                icon.name: "window-close"
            }
            PlasmaComponents3.RadioButton {
                text: i18n("Option")
                checked: true
            }
            PlasmaComponents3.RadioButton {
                text: i18n("Option")
                checked: false
            }
            PlasmaComponents3.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: model.imagePath
                visible: width < background.width
            }
        }
    }
}
