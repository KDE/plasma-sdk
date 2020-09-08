/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

    readonly property var iconSizes: [Kirigami.Units.iconSizes.small / 2,
        Kirigami.Units.iconSizes.small, Kirigami.Units.iconSizes.smallMedium, Kirigami.Units.iconSizes.medium,
        Kirigami.Units.iconSizes.large, Kirigami.Units.iconSizes.huge, Kirigami.Units.iconSizes.enormous]

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
