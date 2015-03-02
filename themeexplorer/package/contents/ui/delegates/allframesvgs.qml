/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    id: delegateRoot
    property var prefixes: model.frameSvgPrefixes
    property var imagePath: model.imagePath
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
    Flow {
        id: flow
        clip: true
        anchors {
            fill: parent
            margins: units.gridUnit * 2
        }
        Repeater {
            model: delegateRoot.prefixes
            delegate: PlasmaCore.FrameSvgItem {
                width: flow.width / 2
                height: flow.height / 3
                imagePath: delegateRoot.imagePath
                prefix: modelData
                PlasmaComponents.Label {
                    anchors.centerIn: parent
                    text: modelData
                    visible: width < parent.width
                }

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
        }
    }

    PlasmaComponents.Label {
        anchors {
            horizontalCenter: background.horizontalCenter
            bottom: background.bottom
        }
        text: model.imagePath
        visible: width < flow.width
    }
}
