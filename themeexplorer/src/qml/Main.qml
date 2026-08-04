/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kitemmodels as KItemModels

import org.kde.ki18n

import org.kde.plasma.themeexplorer

Kirigami.AbstractApplicationWindow {
    id: root
    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 35
    visible: true
    property int iconSize: iconSizeSlider.value
    property alias showMargins: showMarginsCheckBox.checked

    title: KI18n.i18nc("@title:window", "Plasma Theme Explorer")

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    header: QQC2.ToolBar {
        RowLayout {
            anchors.fill: parent
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("New Theme…")
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
                text: KI18n.i18n("Theme:")
            }
            QQC2.ComboBox {
                id: themeSelector
                //FIXME: why crashes?
                //model: 3//ThemeModel.themeList
                textRole: "display"
                onCurrentIndexChanged: {
                    ThemeModel.theme = ThemeModel.themeList.get(currentIndex).packageNameRole;
                }
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Open Folder")
                icon.name: "document-open-folder"
                onClicked: Qt.openUrlExternally(ThemeModel.themeFolder);
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Edit Metadata…")
                icon.name: "configure"
                enabled: view.currentItem?.modelData.isWritable ?? false
                onClicked: {
                    if (!root.metadataEditor) {
                        root.metadataEditor = metadataEditorComponent.createObject(root);
                    }
                    root.metadataEditor.newTheme = false;
                    root.metadataEditor.name = ThemeModel.theme;
                    root.metadataEditor.author = ThemeModel.author;
                    root.metadataEditor.email = ThemeModel.email;
                    root.metadataEditor.license = ThemeModel.license;
                    root.metadataEditor.website = ThemeModel.website;
                    root.metadataEditor.open();
                }
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Edit Colors…")
                icon.name: "color"
                enabled: view.currentItem?.modelData.isWritable ?? false
                onClicked: {
                    let colorEditor = Qt.createComponent("org.kde.plasma.themeexplorer", "ColorEditor").createObject(root) as ColorEditor;
                    colorEditor.open();
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Help")
                icon.name: "help-contents"
                onClicked: Qt.openUrlExternally("https://techbase.kde.org/Development/Tutorials/Plasma5/ThemeDetails");
            }
            QQC2.TextField {
                placeholderText: KI18n.i18n("Search…")
                onTextChanged: searchModel.filterRegularExpression = RegExp(".*" + text + ".*")
            }
        }
    }

    property QtObject metadataEditor
    Component {
        id: metadataEditorComponent
        MetadataEditor {}
    }

    Timer {
        running: true
        interval: 200
        onTriggered: {
            themeSelector.model = ThemeModel.themeList
            for (let i = 0; i < ThemeModel.themeList.count; ++i) {
                if (commandlineTheme == ThemeModel.themeList.get(i).packageNameRole) {
                    themeSelector.currentIndex = i;
                    break;
                }
            }
            //NOTE:assigning this in a second moment solves a crash in some versions of Qt 5.8
            searchModel.sourceModel= ThemeModel
        }
    }
    SystemPalette {
        id: palette
    }

    Rectangle {
        anchors.fill: scrollView
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
    }
    QQC2.ScrollView {
        id: scrollView
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: sidebar.left
        }
        GridView {
            id: view
            anchors.fill: parent
            model: KItemModels.KSortFilterProxyModel {
                id: searchModel
                filterRoleName: "imagePath"
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
            // TODO color: palette.highlight
        }
        ColumnLayout {
            anchors {
                fill: parent
                margins: Kirigami.Units.gridUnit
            }
            QQC2.Label {
                Layout.fillWidth: true
                visible: !view.currentItem?.modelData.isWritable ?? false
                text: KI18n.i18n("This is a readonly, system wide installed theme")
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: KI18n.i18n("Preview:")
            }
            Loader {
                id: extendedLoader
                property QtObject model: view.currentItem?.modelData ?? null
                Layout.fillWidth: true
                Layout.minimumHeight: width
                source: model ? Qt.resolvedUrl("delegates/" + model.delegate + ".qml") : ""
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: KI18n.i18n("Image path: %1", view.currentItem?.modelData.imagePath ?? KI18n.i18n("None"))
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: KI18n.i18n("Description: %1", view.currentItem?.modelData.description ?? "")
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: view.currentItem && view.currentItem.modelData.usesFallback ? KI18n.i18n("Missing from this theme") : KI18n.i18n("Present in this theme")
                wrapMode: Text.WordWrap
            }
            QQC2.CheckBox {
                id: showMarginsCheckBox
                text: i18n("Show Margins")
            }
            QQC2.Button {
                text: view.currentItem && view.currentItem.modelData.usesFallback ? KI18n.i18n("Create with Editor…") : KI18n.i18n("Open In Editor…")
                enabled: view.currentItem?.modelData.isWritable ?? false
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    print(view.currentItem.modelData.svgAbsolutePath)
                    ThemeModel.editElement(view.currentItem.modelData.imagePath)
                    //Qt.openUrlExternally(view.currentItem.modelData.svgAbsolutePath)
                }
            }
            QQC2.Slider {
                id: iconSizeSlider
                Layout.fillWidth: true
                value: Kirigami.Units.gridUnit * 12
                from: Kirigami.Units.gridUnit * 5
                to: Kirigami.Units.gridUnit * 20
            }
        }
    }
}
