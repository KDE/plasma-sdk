/*
 *   SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.14

import org.kde.draganddrop 2.0 as DragAndDrop
import org.kde.kirigami 2.12 as Kirigami
import org.kde.plasma.sdk 1.0 as SDK

Kirigami.ApplicationWindow {
    id: root
    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 26
    visible: true

    Component.onCompleted: {
        for (let i in SDK.GlobalTheme.themeList) {
            if (commandlineTheme == SDK.GlobalTheme.themeList.get(i).packageNameRole) {
                themeSelector.currentIndex = i;
                break;
            }
        }
    }

    property QtObject metadataEditor

    Component {
        id: metadataEditorComponent
        MetadataEditor {}
    }

    pageStack.initialPage: Kirigami.ScrollablePage {
        titleDelegate: ComboBox {
            id: themeSelector
            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth + 1 // The +1 is to make sure we do not trigger eliding at max width
            Layout.minimumWidth: 0
            model: SDK.GlobalTheme.themeList
            textRole: "displayRole"
            onCurrentIndexChanged: {
                SDK.GlobalTheme.theme = SDK.GlobalTheme.themeList.get(currentIndex).packageNameRole;
            }
        }
        actions.main: Kirigami.Action {
            text: i18n("New Theme...")
            iconName: "document-new"
            onTriggered: {
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
        actions.contextualActions: [
            Kirigami.Action {
                text: i18n("Open Theme Folder")
                iconName: "document-open-folder"
                onTriggered: Qt.openUrlExternally(`file://${SDK.GlobalTheme.themeFolder}`);
            },
            Kirigami.Action {
                text: i18n("Save")
                iconName: "document-save"
                onTriggered: SDK.GlobalTheme.save()
                enabled: SDK.GlobalTheme.needsSave
            }
        ]
        GridLayout {
            columns: width > formField.width * 2 + Kirigami.Units.gridUnit ? 2 : 1

            Layout.alignment: Qt.AlignHCenter
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Kirigami.FormLayout {
                    id: form
                    enabled: SDK.GlobalTheme.isWritable
                    FormField {
                        id: formField
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
                    CheckBox {
                        onClicked: SDK.GlobalTheme.performLayoutDump = checked
                        checked: SDK.GlobalTheme.performLayoutDump
                        text: i18n("Use layout from current Plasma setup")
                    }
                    CheckBox {
                        onClicked: SDK.GlobalTheme.performDefaultsDump = checked
                        checked: SDK.GlobalTheme.performDefaultsDump
                        text: i18n("Use configuration from current Plasma setup")
                    }
                }
            }
            Connections {
                target: SDK.GlobalTheme
                onThumbnailPathChanged: {
                    thumbnail.source = ""
                    thumbnail.source = "file://" + SDK.GlobalTheme.thumbnailPath
                }
                onMessageRequested: {
                    root.showPassiveNotification(message);
                }
            }
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Kirigami.Units.gridUnit
                Layout.preferredWidth: formField.width

                Image {
                    id: thumbnail
                    fillMode: Image.PreserveAspectFit
                    Layout.fillWidth: true

                    cache: false
                    DragAndDrop.DropArea {
                        id: dropArea
                        anchors.fill: parent
                        onDrop: {
                            if (event.mimeData.urls[0]) {
                                SDK.GlobalTheme.processThumbnail(event.mimeData.urls[0]);
                            }
                            event.accept(Qt.CopyAction);
                            thumbnail.sourceChanged(thumbnail.source);
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            SDK.GlobalTheme.processThumbnail(SDK.GlobalTheme.openFile());
                            thumbnail.sourceChanged(thumbnail.source);
                        }
                    }
                }
                QQC2.Label {
                    text: i18n("Click to select an image")
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }
}
