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
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtras

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

    PlasmaComponents3.ScrollView {
        anchors.fill: background
        contentWidth: background.width * 2
        contentHeight: background.width * 2
    }

    PlasmaComponents.Label {
        anchors.centerIn: background
        text: model.imagePath
        visible: width < background.width
    }
}
