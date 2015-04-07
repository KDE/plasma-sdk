/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

import QtQuick 2.3
import QtQuick.Controls 1.2 as QtControls

SaveSystemItem {
    id: branch
    icon: "system-switch-user"

    MouseArea {
        anchors.fill: branch
        onClicked: {
            menu.popup()
        }
    }

    QtControls.Menu {
        id: menu
        QtControls. MenuItem {
            text: i18n("Create New Branch")
            onTriggered: stackView.push(Qt.resolvedUrl("BranchView/NewBranchView.qml"))
        }

        QtControls. MenuItem {
            text: i18n("Switch to Branch")
            onTriggered: stackView.push(Qt.resolvedUrl("BranchView/SwitchBranchView.qml"))
        }

        QtControls. MenuItem {
            text: i18n("Rename Branch")
            onTriggered: stackView.push(Qt.resolvedUrl("BranchView/RenameBranchView.qml"))
        }

        QtControls. MenuSeparator {}

        QtControls. MenuItem {
            text: i18n("Delete Branch")
            onTriggered: stackView.push(Qt.resolvedUrl("BranchView/DeleteBranchView.qml"))
        }

    }
}

