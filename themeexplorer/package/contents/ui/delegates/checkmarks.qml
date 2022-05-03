/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

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
            anchors.centerIn: background
            PlasmaComponents.CheckBox {
                text: i18n("Option")
                checked: true
            }
            PlasmaComponents.CheckBox {
                text: i18n("Option")
                checked: false
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
                text: model.imagePath
                visible: width < background.width
            }
        }
    }
}
