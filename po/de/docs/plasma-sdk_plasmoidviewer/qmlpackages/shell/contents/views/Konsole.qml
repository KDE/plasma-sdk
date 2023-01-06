/*
 *   SPDX-FileCopyrightText: 2013 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
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
