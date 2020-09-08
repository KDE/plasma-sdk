/*
 *  SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Layouts 1.0
import org.kde.kquickcontrols 2.0

Item {
    id: root
    width: childrenRect.width
    height: childrenRect.height

    signal configurationChanged
    function saveConfig() {
        plasmoid.globalShortcut = button.keySequence
    }

    ColumnLayout {
        anchors {
            left: parent.left
            right: parent.right
        }
        QtControls.Label {
            Layout.fillWidth: true
            text: i18nd("plasma_shell_org.kde.plasma.desktop", "This shortcut will activate the applet: it will give the keyboard focus to it, and if the applet has a popup (such as the start menu), the popup will be open.")
            wrapMode: Text.WordWrap
        }
        KeySequenceItem {
            id: button
            keySequence: plasmoid.globalShortcut
            onKeySequenceChanged: {
                root.configurationChanged();
            }
        }
    }
}
