/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
 *   Copyright 2019      Carson Black <uhhadd@gmail.com>                   *
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.5 as QQC2
import Qt.labs.settings 1.0

import org.kde.kirigami 2.4 as Kirigami

Kirigami.ApplicationWindow {
    id: cuttlefish
    objectName: "cuttlefish"

    signal itemRightClicked()

    minimumWidth: 400
    minimumHeight: 400

    property bool widescreen: cuttlefish.width >= 800

    property int iconSize: Kirigami.Units.iconSizes.large

    property alias actions: actions.actions

    Loader {
        // Use a Loader instead of creating a GMB directly,
        // so if the GMB errors, it doesn't affect Cuttlefish's operation
        id: loader
        source: "GlobalMenuBar.qml"
    }
    Actions {
        id: actions
    }

    Menu {
        id: menu
    }
    Settings {
        property alias x: cuttlefish.x
        property alias y: cuttlefish.y
        property alias width: cuttlefish.width
        property alias height: cuttlefish.height
    }
    Connections {
        target: loader.item
        onQuit: { cuttlefish.close() }
        onZoomIn: { tools.slider.value += 1 }
        onZoomOut: { tools.slider.value -= 1 }
        onMontage: {
            switch(type) {
                case 0:
                    previewPane.iconPreview.shot("active")
                    break;
                case 1:
                    previewPane.iconPreview.shot("normal")
                    break;
                case 2:
                    previewPane.iconPreview.shot("dark")
                    break;
                case 3:
                    previewPane.dualMont.shot()
                    break;
            }
        }
    }

    function indexToSize(ix) {

            var sizes = new Array();
            sizes[0] = 8;
            sizes[1] = 16;
            sizes[2] = 22;
            sizes[3] = 32;
            sizes[4] = 48
            sizes[5] = 64;
            sizes[6] = 128;

            return sizes[ix];
    }
    Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
    }
    ColumnLayout {
        Shortcut {
            sequence: StandardKey.ZoomIn
            onActivated: tools.slider.value += 1
        }
        Shortcut {
            sequence: StandardKey.ZoomOut
            onActivated: tools.slider.value -= 1
        }
        anchors.fill: parent
        spacing: 0

        Tools {
            id: tools
            Layout.fillWidth: true
            Layout.preferredHeight: (Kirigami.Units.gridUnit * 2) + Kirigami.Units.largeSpacing
        }

        RowLayout {
            QQC2.ScrollView {
                id: grid
                Layout.fillWidth: true
                Layout.fillHeight: true
                QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
                IconGrid {
                    id: iconGrid
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.gridUnit
                }
            }
            Preview {
                visible: cuttlefish.widescreen
                id: previewPane
                Layout.preferredWidth: Kirigami.Units.gridUnit * 22
                Layout.fillHeight: true
            }
        }
        ToolsResponsive {
            visible: !cuttlefish.widescreen
            id: toolsResponsive
            Layout.fillWidth: true
            Layout.preferredHeight: (Kirigami.Units.gridUnit * 2) + Kirigami.Units.largeSpacing
        }
    }
    Loader {
        active: !cuttlefish.widescreen
        source: "ResponsivePreview.qml"
    }
    Comparison {
        id: comparison
    }
    Connections {
        target: tools
        onColorschemeChanged: (index) => {
            toolsResponsive.currentIndex = index
        }
        onSliderValueChanged: (val) => {
            toolsResponsive.value = val
        }
    }
    Connections {
        target: toolsResponsive
        onColorschemeChanged: (index) => {
            tools.currentIndex = index
        }
        onSliderValueChanged: (val) => {
            tools.value = val
        }
    }
    Item {
        id: preview
        property string iconName: "plasma"
        property string fullPath: ""
        property string category: ""
        property string fileName: ""
        property string type: ""
        property string iconTheme: ""
        property variant sizes: []
        property bool scalable: true
    }
}
