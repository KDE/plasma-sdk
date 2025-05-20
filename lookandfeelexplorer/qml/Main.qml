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
import org.kde.plasma.lookandfeelexplorer

Kirigami.ApplicationWindow {
    id: root

    readonly property bool wideMode: root.width >= Kirigami.Units.gridUnit * 50


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

                    model: LnfLogic.lnfList
                    textRole: "displayRole"
                    valueRole: "packageNameRole"
                    currentIndex: {
                        let index = indexOfValue(LnfLogic.them);
                        if (index === -1) {
                            index = 0;
                        }
                        return index;
                    }

                    onCurrentIndexChanged: {
                        if (currentIndex >= 0) {
                            LnfLogic.theme = LnfLogic.lnfList.get(currentIndex).package;
                        }
                    }

                    Layout.fillWidth: true
                }
            }

            QQC2.ItemDelegate {
                text: i18n("New Theme…")
                icon.name: "document-new-symbolic"
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
                icon.name: "document-open-folder-symbolic"
                onClicked: Qt.openUrlExternally('file:' + LnfLogic.themeFolder);
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
        title: LnfLogic.name

        Kirigami.FormLayout {
            enabled: LnfLogic.isWritable

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
                    source: 'file:' + LnfLogic.thumbnailPath

                    cache: false
                    DragAndDrop.DropArea {
                        id: dropArea
                        anchors.fill: parent
                        onDrop: event => {
                            if (event.mimeData.urls[0]) {
                                LnfLogic.processThumbnail(event.mimeData.urls[0]);
                            }
                            event.accept(Qt.CopyAction);
                        }
                    }
                    MouseArea {
                        anchors.fill:parent
                        onClicked: {
                            LnfLogic.processThumbnail(LnfLogic.openFile());
                        }
                    }
                }
            }

            QQC2.Label {
                text: i18n("Plugin name: %1", LnfLogic.theme)
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
                label: i18n("Website:")
                key: "website"
            }
            FormField {
                label: i18n("License:")
                key: "license"
            }
            QQC2.Button {
                text: i18n("Layout from current Plasma setup")
                onClicked: LnfLogic.performLayoutDump = true
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18n("Defaults from current setup")
                onClicked: LnfLogic.performDefaultsDump = true
                Layout.fillWidth: true
            }
        }
        Connections {
            target: LnfLogic
            function onThumbnailPathChanged(): void {
                thumbnail.source = ""
                thumbnail.source = 'file:' + LnfLogic.thumbnailPath
            }
            function onMessageRequested(level: int, message: string): void {
                root.showPassiveNotification(message);
            }
        }
        footer: QQC2.ToolBar {
            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
            leftPadding: 0

            contentItem: QQC2.DialogButtonBox {
                QQC2.Button {
                    text: i18n("Save")
                    enabled: LnfLogic.needsSave
                    onClicked: LnfLogic.save()
                }
            }
        }
    }
}
