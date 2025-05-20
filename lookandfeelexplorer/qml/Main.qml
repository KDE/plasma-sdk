/*
 *   SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.draganddrop as DragAndDrop
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: root

    readonly property bool wideMode: root.width >= Kirigami.Units.gridUnit * 50

    Component.onCompleted: {
        for (var i = 0; i < lnfLogic.lnfList.count; ++i) {
            if (commandlineTheme == lnfLogic.lnfList.get(i).packageNameRole) {
                themeSelector.currentIndex = i;
                break;
            }
        }
    }

    globalDrawer: Kirigami.OverlayDrawer {
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: !root.wideMode
        handleVisible: modal
        onModalChanged: drawerOpen = !modal
        width: Kirigami.Units.gridUnit * 14

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: root.pageStack.globalToolBar.preferredHeight

                contentItem: QQC2.ComboBox {
                    id: themeSelector
                    Layout.fillWidth: true
                    model: lnfLogic.lnfList
                    textRole: "displayRole"
                    onCurrentIndexChanged: {
                        lnfLogic.theme = lnfLogic.lnfList.get(currentIndex).packageNameRole;
                    }
                }
            }

            QQC2.ItemDelegate {
                text: i18n("New Theme…")
                icon.name: "document-new"
                Layout.fillWidth: true
                onClicked: {
                    if (!root.metadataEditor) {
                        root.metadataEditor = metadataEditorComponent.createObject(root);
                    }
                    root.metadataEditor.pluginName = "";
                    root.metadataEditor.name = "";
                    root.metadataEditor.comment = "";
                    root.metadataEditor.author = "";
                    root.metadataEditor.email = "";
                    root.metadataEditor.license = "LGPL 2.1+";
                    root.metadataEditor.website = "";
                    root.metadataEditor.open();
                }
            }

            QQC2.ItemDelegate {
                text: i18n("Open Theme Folder")
                icon.name: "document-open-folder"
                onClicked: Qt.openUrlExternally(lnfLogic.themeFolder);
                Layout.fillWidth: true
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    property QtObject metadataEditor
    Component {
        id: metadataEditorComponent

        MetadataEditor {
            anchors.centerIn: parent
        }
    }

    pageStack.initialPage: Kirigami.ScrollablePage {
        title: lnfLogic.name

        Kirigami.FormLayout {
            enabled: lnfLogic.isWritable

            Rectangle {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 20
                implicitHeight: Kirigami.Units.gridUnit * 10

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                color: Kirigami.Theme.backgroundColor
                radius: Kirigami.Units.cornerRadius

                QQC2.Label {
                    anchors.centerIn: parent
                    text: i18n("Click to open an image")
                    visible: thumbnail.source == ""
                }

                Image {
                    id: thumbnail
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit

                    cache: false
                    DragAndDrop.DropArea {
                        id: dropArea
                        anchors.fill: parent
                        onDrop: event => {
                            if (event.mimeData.urls[0]) {
                                lnfLogic.processThumbnail(event.mimeData.urls[0]);
                            }
                            event.accept(Qt.CopyAction);
                            thumbnail.sourceChanged(thumbnail.source);
                        }
                    }
                    MouseArea {
                        anchors.fill:parent
                        onClicked: {
                            lnfLogic.processThumbnail(lnfLogic.openFile());
                            thumbnail.sourceChanged(thumbnail.source);
                        }
                    }
                }
            }

            QQC2.Label {
                text: i18n("Plugin name: %1", lnfLogic.theme)
            }
            FormField {
                label: i18n("Name:")
                key: "name"
            }
            FormField {
                label: i18n("Comment:")
                key: "comment"
            }
            FormField {
                label: i18n("Author:")
                key: "author"
            }
            FormField {
                label: i18n("Email:")
                key: "email"
            }
            FormField {
                label: i18n("Version:")
                key: "version"
            }
            FormField {
                label: i18n("Website:")
                key: "website"
            }
            FormField {
                label: i18n("License:")
                key: "license"
            }
            QQC2.Button {
                text: i18n("Layout from current Plasma setup")
                onClicked: lnfLogic.performLayoutDump = true
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18n("Defaults from current setup")
                onClicked: lnfLogic.performDefaultsDump = true
                Layout.fillWidth: true
            }
        }
        Connections {
            target: lnfLogic
            function onThumbnailPathChanged() {
                thumbnail.source = ""
                thumbnail.source = lnfLogic.thumbnailPath
            }
            function onMessageRequested(level, message) {
                root.showPassiveNotification(message);
            }
        }
        footer: QQC2.ToolBar {
            padding: Kirigami.Units.smallSpacing

            contentItem: QQC2.DialogButtonBox {
                QQC2.Button {
                    text: i18n("Save")
                    enabled: lnfLogic.needsSave
                    onClicked: lnfLogic.save()
                }
            }
        }
    }
}
