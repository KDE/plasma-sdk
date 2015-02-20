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


GridView {
    id: iconGrid

    focus: true

    cellWidth: iconSize + units.gridUnit
    cellHeight: cellWidth + units.gridUnit

    cacheBuffer: 10000
    highlightMoveDuration: 0

    model: iconModel.svgIcons

    highlight: Rectangle {
        color: theme.highlightColor
        //height: parent.height + units.gridUnit * 3
        anchors.bottomMargin: -units.gridUnit * 2
    }

    delegate: MouseArea {
        id: delegateRoot
        width: iconSize
        height: iconSize + units.gridUnit
        hoverEnabled: hoveredHighlight

        function setAsPreview() {
            print("preview() " + modelData.iconName + " " + modelData.fileName);
//             preview.fullPath = fullPath
            preview.iconName = modelData.iconName

        }

        Rectangle {
            color: theme.highlightColor
            //height: parent.height + units.gridUnit * 3
            opacity: iconGrid.currentIndex == index ? 1.0 : 0
            Behavior on opacity { NumberAnimation { duration: units.shortDuration } }
            anchors {
                bottomMargin: -units.gridUnit
                leftMargin: -units.gridUnit / 2
                rightMargin: -units.gridUnit / 2
                fill: parent
            }
        }

        PlasmaCore.IconItem {
            id: delegateIcon
            width: iconSize
            height: width
            source: modelData.iconName
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }

        PlasmaComponents.Label {
            font.pointSize: iconSize > 96 ? theme.defaultFont.pointSize : theme.smallestFont.pointSize
            text: modelData.fileName + " " + modelData.iconName
            wrapMode: Text.Wrap
//             elide: Text.ElideRight
            maximumLineCount: 3
            horizontalAlignment: Text.AlignHCenter
            opacity: iconGrid.currentIndex == index ? 1.0 : 0.7
            anchors {
                left: parent.left
                right: parent.right
                top: delegateIcon.bottom
                //topMargin: Math.round(-units.gridUnit / 4)
                margins: Math.round(-units.gridUnit / 4)
                //horizontalCenter: parent.horizontalCenter
                //bottom: parent.bottom
            }
        }

        Connections {
            target: iconGrid
            onCurrentIndexChanged: {
                if (delegateRoot.GridView.isCurrentItem) {
                    print("index changed" + modelData.iconName + " " + modelData.fileName)
                    //preview.fullPath = fullPath
                    delegateRoot.setAsPreview();
                }
            }
        }
        onClicked: {
            iconGrid.currentIndex = index;
            iconGrid.forceActiveFocus();
        }
        onEntered: {
            setAsPreview();
        }
        Component.onCompleted: {
//             print("ModelData.fileName:" + modelData.fileName)
//             print("ModelData.iconName:" + modelData.iconName)
        }
    }
}
