/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2017 Sebastian Kügler <sebas@kde.org>                  *
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

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.8 as Kirigami

MouseArea {
    id: delegateRoot
    width: iconSize
    height: iconSize + Math.round(units.gridUnit * 1.25)

    function setAsPreview() {
        preview.fullPath = fullPath
        preview.iconName = iconName
        preview.fileName = fileName
        preview.category = category
        preview.type = type
        preview.iconTheme = iconTheme
        preview.sizes = sizes
        preview.scalable = scalable;
    }

    Rectangle {
        color: theme.highlightColor
        opacity: iconGrid.currentIndex == index ? 1.0 : 0.0
        visible: opacity != 0.0
        Behavior on opacity { NumberAnimation { duration: units.shortDuration } }
        anchors {
            bottomMargin: Math.round(-units.gridUnit * 1.25)
            margins: -units.gridUnit / 2
            fill: parent
        }
    }

    Kirigami.Icon {
        Kirigami.Theme.inherit: true
        id: delegateIcon
        width: iconSize
        height: width
        source: iconName
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    QQC2.Label {
        font.pointSize: iconSize > 96 ? theme.defaultFont.pointSize : theme.smallestFont.pointSize
        text: iconName
        wrapMode: Text.Wrap
        maximumLineCount: 3
        horizontalAlignment: Text.AlignHCenter
        opacity: iconGrid.currentIndex == index ? 1.0 : 0.7
        anchors {
            left: parent.left
            right: parent.right
            top: delegateIcon.bottom
            topMargin: 0
            margins: Math.round(-units.gridUnit / 4)
        }
    }

    Connections {
        target: iconGrid
        onCurrentIndexChanged: {
            if (delegateRoot.GridView.isCurrentItem) {
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
}
