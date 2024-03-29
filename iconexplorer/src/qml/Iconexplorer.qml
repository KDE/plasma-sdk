/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami 2 as Kirigami

Kirigami.ApplicationWindow {
    id: cuttlefish
    objectName: "cuttlefish"

    signal itemRightClicked()

    minimumWidth: Kirigami.Units.gridUnit * 22
    minimumHeight: Kirigami.Units.gridUnit * 22

    property bool widescreen: cuttlefish.width >= Kirigami.Units.gridUnit * 44

    property int iconSize: Kirigami.Units.iconSizes.large

    property alias actions: actions.actions

    readonly property list<int> iconSizes: [
        Kirigami.Units.iconSizes.small / 2,
        Kirigami.Units.iconSizes.small,
        Kirigami.Units.iconSizes.smallMedium,
        Kirigami.Units.iconSizes.medium,
        Kirigami.Units.iconSizes.large,
        Kirigami.Units.iconSizes.huge,
        Kirigami.Units.iconSizes.enormous,
    ]

    function doScreenshot(montageType) {
        ssLoader.type = montageType
        ssLoader.active = true
    }

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
        function onQuit() { cuttlefish.close() }
        function onZoomIn() { tools.slider.value += 1 }
        function onZoomOut() { tools.slider.value -= 1 }
        function onMontage(montageType) {
            cuttlefish.doScreenshot(montageType)
        }
    }

    Shortcut {
        sequences: [StandardKey.ZoomIn]
        onActivated: tools.slider.value += 1
    }
    Shortcut {
        sequences: [StandardKey.ZoomOut]
        onActivated: tools.slider.value -= 1
    }

    header: Tools {
        id: tools
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // To set the scrollview's background color correctly without
        // explicitly adding a frame
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor

            QQC2.ScrollView {
                id: grid
                anchors.fill: parent

                QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

                IconGrid {
                    id: iconGrid
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.gridUnit
                }
            }
        }
        Kirigami.Separator {
            Layout.fillHeight: true
            visible: cuttlefish.widescreen
        }
        Preview {
            id: previewPane
            visible: cuttlefish.widescreen
            Layout.preferredWidth: Kirigami.Units.gridUnit * 22
            Layout.fillHeight: true
        }
    }

    footer: ToolsResponsive {
        id: toolsResponsive
        visible: !cuttlefish.widescreen
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
        function onColorschemeChanged(index) {
            toolsResponsive.currentIndex = index
        }
        function onSliderValueChanged(val) {
            toolsResponsive.value = val
        }
    }
    Connections {
        target: toolsResponsive
        function onColorschemeChanged(index) {
            tools.currentIndex = index
        }
        function onSliderValueChanged(val) {
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
    Loader {
        id: ssLoader
        sourceComponent: Screenshot {}
        active: false

        property int type: Screenshot.MontageType.Dual
        onLoaded: {
            ssLoader.item.shot(type)
        }
        Connections {
            target: ssLoader.item
            function onFinished() {
                ssLoader.active = false
            }
        }
    }
}
