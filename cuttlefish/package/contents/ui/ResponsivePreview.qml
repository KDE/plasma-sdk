/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Carson Black <uhhadd@gmail.com>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.8 as Kirigami

Kirigami.GlobalDrawer {
    id: root
    edge: Qt.RightEdge

    handleOpenIcon.source: "dialog-close"
    handleClosedIcon.source: "view-preview"

    Kirigami.Theme.textColor: cuttlefish.textcolor
    Kirigami.Theme.backgroundColor: cuttlefish.bgcolor
    Kirigami.Theme.highlightColor: cuttlefish.highlightcolor
    Kirigami.Theme.highlightedTextColor: cuttlefish.highlightedtextcolor
    Kirigami.Theme.positiveTextColor: cuttlefish.positivetextcolor
    Kirigami.Theme.neutralTextColor: cuttlefish.neutraltextcolor
    Kirigami.Theme.negativeTextColor: cuttlefish.negativetextcolor

    focus: false

    function clipboard(text) {
        if (!pickerMode) {
            clipboardHelper.text = text;
            clipboardHelper.selectAll();
            clipboardHelper.copy();
        } else {
            iconModel.output(text);
        }
    }

    TextEdit {
        id: clipboardHelper
        visible: false
    }

    actions: [
        Kirigami.Action {
            text: i18n("Open icon with external program")
            iconName: "document-open"
            onTriggered: Qt.openUrlExternally(preview.fullPath)
        },
        Kirigami.Action {
            text: i18n("Copy icon name to clipboard")
            iconName: "edit-copy"
            onTriggered: {
                root.clipboard(preview.iconName)
                cuttlefish.showPassiveNotification(i18n("Icon name copied to clipboard"), "short")
            }
        },
        Kirigami.Action {
            iconName: "camera-web-symbolic"
            text: i18n("Create screenshot of icon with...")
            Kirigami.Action {
                text: i18n("Breeze Colors")
                onTriggered: iconPreview.shot("normal")
            }
            Kirigami.Action {
                text: i18n("Breeze Dark Colors")
                onTriggered: iconPreview.shot("dark")
            }
            Kirigami.Action {
                onTriggered: dualMont.shot()
                text: i18n("Breeze (Normal) and Breeze Dark")
            }
            Kirigami.Action {
                onTriggered: iconPreview.shot("active")
                text: i18n("Active Color Scheme")
            }
        },
        Kirigami.Action {
            text: i18n("View icon in other themes")
            icon.name: "document-equal"
            onTriggered: comparison.sheetOpen = true
        }
    ]
    Kirigami.Heading {
        level: 1
        Layout.fillWidth: true
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        text: preview.iconName
    }
    Kirigami.FormLayout {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing
        QQC2.Label {
            Layout.maximumWidth: Kirigami.Units.gridUnit * 10
            Kirigami.FormData.label: i18n("File name:")
            elide: Text.ElideRight
            text: preview.fileName
        }
        QQC2.Label {
            Kirigami.FormData.label: i18n("Category:")
            font.capitalization: Font.Capitalize
            text: preview.category
        }
        QQC2.Label {
            Kirigami.FormData.label: i18n("Scalable:")
            text: preview.scalable ? i18n("yes") : i18n("no")
            font.capitalization: Font.Capitalize
        }
    }
    GridLayout {
        id: grid
        columns: 2
        property var sizes: [8, 16, 22, 32, 48, 64]
        Layout.alignment: Qt.AlignHCenter
        Repeater {
            model: parent.sizes.length
            delegate: ColumnLayout {
                Kirigami.Icon {
                    Layout.alignment: Qt.AlignBottom
                    source: preview.iconName
                    width: grid.sizes[index]
                    height: grid.sizes[index]
                }
                QQC2.Label {
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    text: grid.sizes[index]
                    Behavior on color {
                        ColorAnimation {
                            duration: Kirigami.Units.longDuration
                        }
                    }
                }
            }
        }
    }
}