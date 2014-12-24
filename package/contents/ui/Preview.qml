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
        PlasmaCore.IconItem {
            source: iconName
            Layout.preferredWidth: units.gridUnit * 10
            Layout.preferredHeight: units.gridUnit * 10
        }
        Image {
            source: fullPath
            Layout.preferredWidth: sourceSize.width * 4
            Layout.preferredHeight: sourceSize.height * 4
        }
    }
    Rectangle {
        color: theme.highlightColor
        width: units.gridUnit / 20
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }


}
