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
    Column {
        anchors.centerIn: background
        PlasmaComponents.TabBar {
            width: background.width - 10
            PlasmaComponents.TabButton {
                text: i18n("Tab1")
            }
            PlasmaComponents.TabButton {
                text: i18n("Tab2")
            }
            PlasmaComponents.TabButton {
                text: i18n("Tab3")
            }
        }
        PlasmaComponents.Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: model.imagePath
            visible: width < background.width
        }
    }
}
