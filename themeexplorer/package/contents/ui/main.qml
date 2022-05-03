/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 1.3
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore

Kirigami.ApplicationWindow {
    id: root

    property int iconSize: iconSizeSlider.value
    property alias showMargins: showMarginsCheckBox.checked

    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 35
    visible: true

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    header: QQC2.ToolBar {
        RowLayout {
            anchors.fill: parent
            QQC2.ToolButton {
                text: i18n("New Theme…")
                icon.name: "document-new"
                onClicked: {
                    if (!root.metadataEditor) {
                        root.metadataEditor = metadataEditorComponent.createObject(root);
                    }
                    root.metadataEditor.newTheme = true;
                    root.metadataEditor.name = "";
                    root.metadataEditor.author = "";
                    root.metadataEditor.email = "";
                    root.metadataEditor.license = "LGPL 2.1+";
                    root.metadataEditor.website = "";
                    root.metadataEditor.open();
                }
            }
            QQC2.Label {
                text: i18n("Theme:")
            }
            QQC2.ComboBox {
                id: themeSelector
                model: themeModel.themeList

                textRole: "display"
                valueRole: "packageNameRole"

                Component.onCompleted: {
                    currentIndex = indexOfValue(commandlineTheme)
                }

                onActivated: {
                    themeModel.theme = currentValue;
                }
            }
            QQC2.ToolButton {
                text: i18n("Open Folder")
                icon.name: "document-open-folder"
                onClicked: Qt.openUrlExternally(themeModel.themeFolder);
            }
            QQC2.ToolButton {
                text: i18n("Edit Metadata…")
                icon.name: "configure"
                enabled: view.currentItem.modelData.isWritable
                onClicked: {
                    if (!root.metadataEditor) {
                        root.metadataEditor = metadataEditorComponent.createObject(root);
                    }
                    root.metadataEditor.newTheme = false;
                    root.metadataEditor.name = themeModel.theme;
                    root.metadataEditor.author = themeModel.author;
                    root.metadataEditor.email = themeModel.email;
                    root.metadataEditor.license = themeModel.license;
                    root.metadataEditor.website = themeModel.website;
                    root.metadataEditor.open();
                }
            }
            QQC2.ToolButton {
                text: i18n("Edit Colors…")
                icon.name: "color"
                enabled: view.currentItem.modelData.isWritable
                onClicked: {
                    if (!root.colorEditor) {
                        root.colorEditor = colorEditorComponent.createObject(root);
                    }

                    root.colorEditor.open();
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            QQC2.ToolButton {
                text: i18n("Help")
                icon.name: "help-contents"
                onClicked: Qt.openUrlExternally("https://techbase.kde.org/Development/Tutorials/Plasma5/ThemeDetails");
            }
            Kirigami.SearchField {
                placeholderText: i18n("Search…")
                onTextEdited: searchModel.setFilterFixedString(text)
            }
        }
    }

    property QtObject metadataEditor
    Component {
        id: metadataEditorComponent
        MetadataEditor {
            onCreated: name => {
                themeSelector.currentIndex = themeSelector.indexOfValue(name);
            }
        }
    }
    property QtObject colorEditor
    Component {
        id: colorEditorComponent
        ColorEditor {}
    }

    SystemPalette {
        id: palette
    }

    QQC2.ScrollView {
        id: scroll

        // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
        QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: sidebar.left
        }
        background: Rectangle {
            color: theme.viewBackgroundColor
        }
        GridView {
            id: view
            anchors.fill: parent
            model: PlasmaCore.SortFilterModel {
                id: searchModel
                sourceModel: themeModel
                filterRole: "imagePath"
            }
            cellWidth: root.iconSize
            cellHeight: cellWidth
            highlightMoveDuration: 0

            highlight: Rectangle {
                radius: 3
                color: palette.highlight
            }
            delegate: Item {
                width: view.cellWidth
                height: view.cellHeight
                property QtObject modelData: model
                MouseArea {
                    z: 2
                    anchors.fill: parent
                    onClicked: {
                        view.currentIndex = index;
                    }
                }
                Loader {
                    z: -1
                    anchors.fill: parent
                    source: Qt.resolvedUrl("delegates/" + model.delegate + ".qml")
                }
                Rectangle {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        margins: Kirigami.Units.gridUnit
                    }
                    width: Kirigami.Units.gridUnit
                    height: Kirigami.Units.gridUnit
                    radius: Kirigami.Units.gridUnit
                    opacity: 0.5
                    color: model.usesFallback ? "red" : "green"
                }
            }
        }
    }
    Item {
        id: sidebar
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        width: root.width / 3
        Rectangle {
            width: 1
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            color: palette.highlight
        }
        ColumnLayout {
            anchors {
                fill: parent
                margins: Kirigami.Units.gridUnit
            }
            Label {
                Layout.fillWidth: true
                visible: !view.currentItem.modelData.isWritable
                text: i18n("This is a readonly, system wide installed theme")
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                text: i18n("Preview:")
            }
            Loader {
                id: extendedLoader
                property QtObject model: view.currentItem.modelData
                Layout.fillWidth: true
                Layout.minimumHeight: width
                source: Qt.resolvedUrl("delegates/" + model.delegate + ".qml")
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Label {
                Layout.fillWidth: true
                text: i18n("Image path: %1", view.currentItem.modelData.imagePath)
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                text: i18n("Description: %1", view.currentItem.modelData.description)
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                text: view.currentItem.modelData.usesFallback ? i18n("Missing from this theme") : i18n("Present in this theme")
                wrapMode: Text.WordWrap
            }
            CheckBox {
                id: showMarginsCheckBox
                text: i18n("Show Margins")
            }
            Button {
                text: view.currentItem.modelData.usesFallback ? i18n("Create with Editor…") : i18n("Open In Editor…")
                enabled: view.currentItem.modelData.isWritable
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    print(view.currentItem.modelData.svgAbsolutePath)
                    themeModel.editElement(view.currentItem.modelData.imagePath)
                    //Qt.openUrlExternally(view.currentItem.modelData.svgAbsolutePath)
                }
            }
            Slider {
                id: iconSizeSlider
                Layout.fillWidth: true
                value: Kirigami.Units.gridUnit * 12
                minimumValue: Kirigami.Units.gridUnit * 5
                maximumValue: Kirigami.Units.gridUnit * 20
            }
        }
    }
}
