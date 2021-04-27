/*
 *  SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root
    Layout.minimumWidth: PlasmaCore.Units.gridUnit * 15
    Layout.minimumHeight: PlasmaCore.Units.gridUnit * 15
    Layout.maximumWidth: PlasmaCore.Units.gridUnit * 15
    Layout.maximumHeight: PlasmaCore.Units.gridUnit * 15

    property var reason
    property var errorInformation: {}

    clip: true

    ColumnLayout {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

        PlasmaCore.IconItem {
            Layout.minimumWidth: PlasmaCore.Units.iconSizes.huge
            Layout.minimumHeight: PlasmaCore.Units.iconSizes.huge
            source: "dialog-error"
            Layout.alignment: Qt.AlignHCenter
        }

        PlasmaExtras.Heading {
            text: i18nd("plasma_shell_org.kde.plasma.desktop", "Sorry! There was an error loading %1.", root.errorInformation.appletName)
            level: 2
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        PlasmaComponents.Button {
            text: i18nd("plasma_shell_org.kde.plasma.desktop", "Copy Error Details to Clipboard")
            icon.name: "edit-copy"

            onClicked: copyHelper.copyText()
            Layout.alignment: Qt.AlignHCenter
        }

        PlasmaComponents.TextArea {
            id: copyHelper
            visible: root.errorInformation.isDebugMode

            text: root.errorInformation.errors.join("\n\n")
            readOnly: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            function copyText() {
                selectAll()
                copy()
            }
        }
    }
}
