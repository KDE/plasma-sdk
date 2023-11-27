/*
 *  SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents

PlasmoidItem {
    id: root
    Layout.minimumWidth: Kirigami.Units.gridUnit * 15
    Layout.minimumHeight: Kirigami.Units.gridUnit * 15
    Layout.maximumWidth: Kirigami.Units.gridUnit * 15
    Layout.maximumHeight: Kirigami.Units.gridUnit * 15

    property var reason
    property var errorInformation

    clip: true

    ColumnLayout {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

        Kirigami.Icon {
            Layout.minimumWidth: Kirigami.Units.iconSizes.huge
            Layout.minimumHeight: Kirigami.Units.iconSizes.huge
            source: "dialog-error"
            Layout.alignment: Qt.AlignHCenter
        }

        Kirigami.Heading {
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
            wrapMode: TextEdit.Wrap
            Layout.fillHeight: true
            Layout.fillWidth: true

            function copyText() {
                selectAll()
                copy()
            }
        }
    }
}
