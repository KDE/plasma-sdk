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
    Column {
        anchors.centerIn: backgroundRect
        PlasmaComponents.ProgressBar {
            width: backgroundRect.width - 10
            value: 0.5
        }
        PlasmaComponents.ProgressBar {
            width: backgroundRect.width - 10
            indeterminate: true
        }
        PlasmaComponents.Label {
            text: root.imagePath
            wrapMode: PlasmaComponents.Label.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
