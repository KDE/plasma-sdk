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
                model: ThemeModel.themeList
                textRole: "display"
                onCurrentIndexChanged: {
                    ThemeModel.theme = ThemeModel.themeList.get(currentIndex).packageNameRole;
                }
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Open Folder")
                icon.name: "document-open-folder"
                onClicked: Qt.openUrlExternally("file://" + ThemeModel.themeFolder);
            }
            QQC2.ToolButton {
                QQC2.ToolTip.text: KI18n.i18n("Edit Metadata…")
                icon.name: "configure"
                enabled: ThemeModel.isWritable
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
                enabled: ThemeModel.isWritable
                onClicked: {
                    let colorEditor = Qt.createComponent("org.kde.plasma.themeexplorer", "ColorEditorDialog").createObject(root) as ColorEditorDialog;
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
                onClicked: Qt.openUrlExternally("https://develop.kde.org/docs/plasma/theme/");
            }
            QQC2.TextField {
                placeholderText: KI18n.i18n("Search…")
                onTextChanged: searchModel.filterRegularExpression = RegExp(".*" + text + ".*")
            }
        }
    }

    property MetadataEditor metadataEditor
    Component {
        id: metadataEditorComponent
        MetadataEditor {}
    }

    Timer {
        running: true
        interval: 200
        onTriggered: {
            for (let i = 0; i < ThemeModel.themeList.count; ++i) {
                if (commandlineTheme == ThemeModel.themeList.get(i).packageNameRole) {
                    themeSelector.currentIndex = i;
                    break;
                }
            }
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

            onCurrentIndexChanged: extendedLoader.update()
            model: KItemModels.KSortFilterProxyModel {
                id: searchModel
                filterRoleName: "imagePath"
                sourceModel: ThemeModel
            }
            cellWidth: root.iconSize
            cellHeight: cellWidth
            highlightMoveDuration: 0

            highlight: Rectangle {
                radius: 3
                color: palette.highlight
            }
            delegate: GridDelegate {
                showMargins: root.showMargins
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
            Loader {
                id: extendedLoader
                Layout.fillWidth: true
                Layout.minimumHeight: width
                source: Qt.resolvedUrl("delegates/" + (view.currentItem as GridDelegate).delegate + ".qml")
                onLoaded: update()
                function update(): void {
                    if (!item) return
                    if ("showMargins" in item) {
                        item.showMargins = Qt.binding(function() {
                            return (view.currentItem as GridDelegate).showMargins
                        });
                    }
                    if ("imagePath" in item) {
                        item.imagePath = (view.currentItem as GridDelegate).imagePath;
                    }
                    if ("frameSvgPrefixes" in item) {
                        item.frameSvgPrefixes = (view.currentItem as GridDelegate).frameSvgPrefixes
                    }
                    if ("iconElements" in item) {
                        item.iconElements = (view.currentItem as GridDelegate).iconElements
                    }
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: KI18n.i18n("Image path: %1", (view.currentItem as GridDelegate).imagePath)
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: KI18n.i18n("Description: %1", (view.currentItem as GridDelegate).description ?? "")
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Layout.fillWidth: true
                text: view.currentItem && (view.currentItem as GridDelegate).usesFallback ? KI18n.i18n("Missing from this theme") : KI18n.i18n("Present in this theme")
                wrapMode: Text.WordWrap
            }
            QQC2.CheckBox {
                id: showMarginsCheckBox
                text: KI18n.i18n("Show Margins")
            }
            QQC2.Button {
                text: view.currentItem && (view.currentItem as GridDelegate).usesFallback ? KI18n.i18n("Create with Editor…") : KI18n.i18n("Open In Editor…")
                enabled: ThemeModel.isWritable
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    ThemeModel.editElement((view.currentItem as GridDelegate).imagePath)
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
