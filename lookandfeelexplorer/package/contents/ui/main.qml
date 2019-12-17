/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.1

import org.kde.draganddrop 2.0 as DragAndDrop
import org.kde.kirigami 2.3 as Kirigami

Kirigami.AbstractApplicationWindow {
    id: root
    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 26
    visible: true

    Component.onCompleted: {
        for (var i = 0; i < lnfLogic.lnfList.count; ++i) {
            if (commandlineTheme == lnfLogic.lnfList.get(i).packageNameRole) {
                themeSelector.currentIndex = i;
                break;
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("Look And Feel")
        titleIcon: "preferences-desktop-theme"
        modal: true;
        collapsible: false;
        collapsed: false;
        topContent: ComboBox {
            id: themeSelector
            Layout.fillWidth: true
            model: lnfLogic.lnfList
            textRole: "displayRole"
            onCurrentIndexChanged: {
                lnfLogic.theme = lnfLogic.lnfList.get(currentIndex).packageNameRole;
            }
        }
        actions: [
            Kirigami.Action {
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
            },
            Kirigami.Action {
                text: i18n("Open Theme Folder")
                iconName: "document-open-folder"
                onTriggered: Qt.openUrlExternally(lnfLogic.themeFolder);
            }
        ]
    }

    property QtObject metadataEditor
    Component {
        id: metadataEditorComponent
        MetadataEditor {}
    }

    SystemPalette {
        id: palette
    }

    RowLayout {
        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing
        }
        Layout.alignment: Qt.AlignHCenter
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Kirigami.FormLayout {
                enabled: lnfLogic.isWritable
                QQC2.Label {
                    text: i18n("Plugin name:") + lnfLogic.theme
                }
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
                QQC2.Button {
                    text: i18n("Layout from current Plasma setup")
                    onClicked: lnfLogic.performLayoutDump = true
                    Layout.columnSpan: 2
                    implicitWidth: formField.width
                }
                QQC2.Button {
                    text: i18n("Defaults from current setup")
                    onClicked: lnfLogic.performDefaultsDump = true
                    Layout.columnSpan: 2
                    implicitWidth: formField.width
                }
            }
        }
        Connections {
            target: lnfLogic
            onThumbnailPathChanged: {
                thumbnail.source = ""
                thumbnail.source = lnfLogic.thumbnailPath
            }
            onMessageRequested: {
                root.showPassiveNotification(message);
            }
        }
        Rectangle {
            width: 250
            height: 250
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
                    onDrop: {
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
    }
    QQC2.Button {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Kirigami.Units.largeSpacing
        }
        text: i18n("Save")
        enabled: lnfLogic.needsSave
        onClicked: lnfLogic.save()
    }
}
