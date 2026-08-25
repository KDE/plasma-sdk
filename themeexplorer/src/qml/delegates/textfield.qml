/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami
import org.kde.ki18n

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
        PlasmaComponents.TextField {
            width: backgroundRect.width - 10
            text: KI18n.i18n("Text")
        }
        PlasmaComponents.Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.imagePath
            visible: width < backgroundRect.width
        }
    }
}
