/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.2
// import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras


Rectangle {

    color: theme.backgroundColor

    property string iconName: "plasma"
    property string fullPath: ""

    ColumnLayout {

        anchors.leftMargin: units.gridUnit
        anchors.rightMargin: units.gridUnit

        anchors.fill: parent
        PlasmaExtras.Heading {
            level: 3
            elide: Text.ElideRight
            Layout.fillWidth: true
            Layout.fillHeight: false
            text: iconName
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: indexToSize(4)

            PlasmaCore.IconItem {
                source: iconName
                Layout.preferredWidth: indexToSize(0)
                Layout.preferredHeight: indexToSize(0)
            }
            PlasmaCore.IconItem {
                source: iconName
                Layout.preferredWidth: indexToSize(1)
                Layout.preferredHeight: indexToSize(1)
            }
            PlasmaCore.IconItem {
                source: iconName
                Layout.preferredWidth: indexToSize(2)
                Layout.preferredHeight: indexToSize(2)
            }
            PlasmaCore.IconItem {
                source: iconName
                Layout.preferredWidth: indexToSize(3)
                Layout.preferredHeight: indexToSize(3)
            }
            PlasmaCore.IconItem {
                source: iconName
                Layout.preferredWidth: indexToSize(4)
                Layout.preferredHeight: indexToSize(4)
            }
        }

        PlasmaCore.IconItem {
            source: iconName
            Layout.fillWidth: true
            Layout.preferredWidth: indexToSize(5)
            Layout.preferredHeight: indexToSize(5)
        }
        PlasmaCore.IconItem {
            source: iconName
            Layout.fillHeight: false
            Layout.preferredWidth: indexToSize(6)
            Layout.preferredHeight: indexToSize(6)
        }
        PlasmaCore.IconItem {
            source: iconName
            Layout.fillHeight: false
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.width
        }
        RowLayout {

            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: indexToSize(4)


            Image {
                source: fullPath
                Layout.fillWidth: false
                Layout.preferredWidth: sourceSize.width
                Layout.preferredHeight: sourceSize.height
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }

    Rectangle {
        color: theme.highlightColor
        width: Math.round(units.gridUnit / 20)
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }
}
