/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
 *   Copyrihgt 2019      Carson Black <uhhadd@gmail.com>                   *
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
    property bool darkScheme: false
    property bool usesPlasmaTheme: true
    property var schemeStash

    property color textcolor: Kirigami.Theme.textColor
    property color bgcolor: Kirigami.Theme.backgroundColor
    property color highlightcolor: Kirigami.Theme.highlightColor
    property color highlightedtextcolor: Kirigami.Theme.highlightedTextColor
    property color positivetextcolor: Kirigami.Theme.positiveTextColor
    property color neutraltextcolor: Kirigami.Theme.neutralTextColor
    property color negativetextcolor: Kirigami.Theme.negativeTextColor
    property color viewbgcolor: Kirigami.Theme.viewBackgroundColor

    Kirigami.Theme.textColor: cuttlefish.textcolor
    Kirigami.Theme.backgroundColor: cuttlefish.bgcolor
    Kirigami.Theme.highlightColor: cuttlefish.highlightcolor
    Kirigami.Theme.highlightedTextColor: cuttlefish.highlightedtextcolor
    Kirigami.Theme.positiveTextColor: cuttlefish.positivetextcolor
    Kirigami.Theme.neutralTextColor: cuttlefish.neutraltextcolor
    Kirigami.Theme.negativeTextColor: cuttlefish.negativetextcolor
    
    Loader {
        // Use a Loader instead of creating a GMB directly,
        // so if the GMB errors, it doesn't affect Cuttlefish's operation
        id: loader
        source: "GlobalMenuBar.qml"
    }
    Loader {
        // Ditto for the right click context menu.
        source: "Menu.qml"
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
    function changeColors(index) {
        switch(index) {
        case 0:
            cuttlefish.Kirigami.Theme.inherit = true
            cuttlefish.textcolor            = cuttlefish.schemeStash[0]
            cuttlefish.bgcolor      = cuttlefish.schemeStash[1]
            cuttlefish.highlightcolor       = cuttlefish.schemeStash[2]
            cuttlefish.highlightedtextcolor = cuttlefish.schemeStash[3]
            cuttlefish.positivetextcolor    = cuttlefish.schemeStash[4]
            cuttlefish.neutraltextcolor     = cuttlefish.schemeStash[5]
            cuttlefish.negativetextcolor    = cuttlefish.schemeStash[6]
            cuttlefish.viewbgcolor = cuttlefish.schemeStash[7]
            break
        case 1:
            cuttlefish.Kirigami.Theme.inherit = false
            cuttlefish.textcolor = "#232629"
            cuttlefish.bgcolor = "#eff0f1"
            cuttlefish.highlightcolor = "#3daee9"
            cuttlefish.highlightedtextcolor = "#eff0f1"
            cuttlefish.positivetextcolor = "#27ae60"
            cuttlefish.neutraltextcolor = "#f67400"
            cuttlefish.negativetextcolor = "#da4453"
            cuttlefish.viewbgcolor = "#fcfcfc"
            break
        case 2:
            cuttlefish.Kirigami.Theme.inherit = false
            cuttlefish.textcolor = "#eff0f1"
            cuttlefish.bgcolor = "#31363b"
            cuttlefish.highlightcolor = "#3daee9"
            cuttlefish.highlightedtextcolor = "#eff0f1"
            cuttlefish.positivetextcolor = "#27ae60"
            cuttlefish.neutraltextcolor = "#f67400"
            cuttlefish.negativetextcolor = "#da4453"
            cuttlefish.viewbgcolor = "#232629"
            break
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
        color: tools.currentIndex != 0 ? cuttlefish.viewbgcolor : Kirigami.Theme.viewBackgroundColor
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

                    footer: SvgGrid {
                        id: svgGrid
                        interactive: false
                    }
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
    Connections {
        target: tools
        onColorschemeChanged: (index) => {
            cuttlefish.changeColors(index)
            toolsResponsive.currentIndex = index
        }
    }
    Connections {
        target: toolsResponsive
        onColorschemeChanged: (index) => {
            cuttlefish.changeColors(index)
            tools.currentIndex = index
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

    Component.onCompleted: {
        cuttlefish.schemeStash += Kirigami.Theme.textColor
        cuttlefish.schemeStash += Kirigami.Theme.backgroundColor
        cuttlefish.schemeStash += Kirigami.Theme.highlightColor
        cuttlefish.schemeStash += Kirigami.Theme.highlightedTextColor
        cuttlefish.schemeStash += Kirigami.Theme.positiveTextColor 
        cuttlefish.schemeStash += Kirigami.Theme.neutralTextColor 
        cuttlefish.schemeStash += Kirigami.Theme.negativeTextColor
        cuttlefish.schemeStash += Kirigami.Theme.viewBackgroundColor
        console.log(cuttlefish.schemeStash)
    }
}
