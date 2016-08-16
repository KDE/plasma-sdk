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
import QtQuick.Layouts 1.1

import org.kde.draganddrop 2.0 as DragAndDrop
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 1.0 as Kirigami

Kirigami.AbstractApplicationWindow {
    id: root
    width: units.gridUnit * 50
    height: units.gridUnit * 26
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
        topContent: ComboBox {
            id: themeSelector
            anchors {
                left: parent.left
                right: parent.right
            }
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
        enabled: lnfLogic.isWritable
        anchors {
            fill: parent
            margins: 10
        }
        GridLayout {
            columns: 2
            Layout.alignment: Qt.AlignHCenter|Qt.AlignVCenter
            FormLabel {
                text: i18n("Plugin Name:")
            }
            Kirigami.Label {
                text: lnfLogic.theme
            }
            FormLabel {
                text: i18n("Name:")
                buddy: nameField
            }
            FormField {
                key: "name"
            }
            FormLabel {
                text: i18n("Comment:")
                buddy: nameField
            }
            FormField {
                key: "comment"
            }
            FormLabel {
                text: i18n("Author:")
                buddy: nameField
            }
            FormField {
                key: "author"
            }
            FormLabel {
                text: i18n("Email:")
                buddy: nameField
            }
            FormField {
                key: "email"
            }
            FormLabel {
                text: i18n("Version:")
                buddy: nameField
            }
            FormField {
                key: "version"
            }
            FormLabel {
                text: i18n("Website:")
                buddy: nameField
            }
            FormField {
                key: "website"
            }
            FormLabel {
                text: i18n("License:")
                buddy: nameField
            }
            FormField {
                key: "license"
            }
            Button {
                text: i18n("Layout from current Plasma setup")
                onClicked: lnfLogic.dumpCurrentPlasmaLayout();
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignRight
            }
            Button {
                text: i18n("Defaults from current setup")
                onClicked: lnfLogic.dumpDefaultsConfigFile(lnfLogic.theme);
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignRight
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
            Kirigami.Label {
                anchors.centerIn: parent
                text: i18n("click to open an image")
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
}
