/*
 *   Copyright 2013 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents


Item {
    id: root

    RowLayout {
        id: buttonRow
        spacing: 0
        anchors{
            top: parent.top
        }

        PlasmaComponents.Button {
            id: saveButton
            iconSource: "document-save-as"
        }
        PlasmaComponents.Button {
            id: clearButton
            iconSource: "edit-clear"
            onClicked: outputArea.text = ""
        }
    }

    PlasmaComponents.TextArea {
        id: outputArea
        width: parent.width
        height: parent.height/2
        anchors.bottom: parent.bottom
        anchors.top: buttonRow.bottom
        anchors.topMargin: 10
    }
}
