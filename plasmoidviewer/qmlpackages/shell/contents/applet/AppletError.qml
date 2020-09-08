/*
 *  SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

RowLayout {
    id: root
    Layout.minimumWidth: units.gridUnit * 20
    Layout.minimumHeight: units.gridUnit * 8

    property alias reason: messageText.text

    clip: true

    PlasmaCore.IconItem {
        id: icon
        anchors.verticalCenter: parent.verticalCenter
        Layout.minimumWidth: units.iconSizes.huge
        Layout.minimumHeight: units.iconSizes.huge
        source: "dialog-error"
    }

    PlasmaComponents.TextArea {
        id: messageText
        Layout.fillWidth: true
        Layout.fillHeight: true
        verticalAlignment: TextEdit.AlignVCenter
        backgroundVisible: false
        readOnly: true
        width: parent.width - icon.width
        wrapMode: Text.Wrap
    }
}
