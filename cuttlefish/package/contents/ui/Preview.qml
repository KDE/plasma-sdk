/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2017 Sebastian Kügler <sebas@kde.org>                  *
 *   Copyright 2019      Carson Black <uhhadd@gmail.com>                   *
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

import QtQuick 2.2
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.8 as Kirigami

Rectangle {
    property alias iconPreview: iconPreview
    property alias dualMont: dualMont

    color: Kirigami.Theme.backgroundColor

    FileDialog {
        id: ssPicker
        selectExisting: false
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            iconPreview.grabToImage(function(result) {
                ssPicker.resetColors()
                res = result.saveToFile(ssPicker.fileUrl.toString().slice(7))
            });
        }
        onRejected: {
            ssPicker.resetColors()
        }
        function resetColors() {
            iconPreview.screenshotting = false
            iconPreview.Kirigami.Theme.textColor            = Kirigami.Theme.textColor
            iconPreview.Kirigami.Theme.backgroundColor      = Kirigami.Theme.backgroundColor
            iconPreview.Kirigami.Theme.highlightColor       = Kirigami.Theme.highlightColor
            iconPreview.Kirigami.Theme.highlightedTextColor = Kirigami.Theme.highlightedTextColor
            iconPreview.Kirigami.Theme.positiveTextColor    = Kirigami.Theme.positiveTextColor
            iconPreview.Kirigami.Theme.neutralTextColor     = Kirigami.Theme.neutralTextColor
            iconPreview.Kirigami.Theme.negativeTextColor    = Kirigami.Theme.negativeTextColor
            iconPreview.Kirigami.Theme.inherit = true
        }
        nameFilters: [ "PNG screenshot files (*.png)" ]
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

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
                Layout.maximumWidth: Kirigami.Units.gridUnit * 15
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
        Rectangle {
            id: iconPreview

            property bool screenshotting: false

            color: Kirigami.Theme.backgroundColor

            Behavior on color {
                ColorAnimation {
                    duration: Kirigami.Units.longDuration
                }
            }
            Behavior on Layout.preferredHeight {
                NumberAnimation { 
                    duration: Kirigami.Units.longDuration
                }
            }
            
            Layout.fillWidth: true
            Layout.preferredHeight: screenshotting ? previewGrid.height + (Kirigami.Units.gridUnit * 4) : previewGrid.height

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing * 4

            ColumnLayout {
                id: previewGrid
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                property var sizes: [8, 16, 22, 32, 48, 64]
                property var largeSizes: [128]
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 2
                    columns: previewGrid.sizes.length
                    Repeater {
                        model: previewGrid.sizes.length
                        delegate: Kirigami.Icon {
                            Layout.alignment: Qt.AlignBottom
                            source: preview.iconName
                            width: previewGrid.sizes[index]
                            height: previewGrid.sizes[index]
                        }
                    }
                    Repeater {
                        model: previewGrid.sizes.length
                        delegate: QQC2.Label {
                            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                            text: previewGrid.sizes[index]
                            Behavior on color {
                                ColorAnimation {
                                    duration: Kirigami.Units.longDuration
                                }
                            }
                        }
                    }
                }
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 2
                    columns: previewGrid.largeSizes.length
                    Repeater {
                        model: previewGrid.largeSizes.length
                        delegate: Kirigami.Icon {
                            Layout.alignment: Qt.AlignBottom
                            source: preview.iconName
                            width: previewGrid.largeSizes[index]
                            height: previewGrid.largeSizes[index]
                        }
                    }
                    Repeater {
                        model: previewGrid.largeSizes.length
                        delegate: QQC2.Label {
                            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                            text: previewGrid.largeSizes[index]
                            Behavior on color {
                                ColorAnimation {
                                    duration: Kirigami.Units.longDuration
                                }
                            }
                        }
                    }
                }
            }
            Row {
                opacity: iconPreview.screenshotting ? 1 : 0
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: Kirigami.Units.smallSpacing
                Kirigami.Icon {
                    height: 32
                    width: 32
                    source: "cuttlefish"
                }
                QQC2.Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Montage made with Cuttlefish"
                }
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Kirigami.Units.longDuration
                    }
                }
            }
            function shot(type) {
                iconPreview.screenshotting = true
                if (type == "normal") {
                    iconPreview.Kirigami.Theme.inherit = false
                    iconPreview.Kirigami.Theme.textColor = "#232629"
                    iconPreview.Kirigami.Theme.backgroundColor = "#eff0f1"
                    iconPreview.Kirigami.Theme.highlightColor = "#3daee9"
                    iconPreview.Kirigami.Theme.highlightedTextColor = "#eff0f1"
                    iconPreview.Kirigami.Theme.positiveTextColor = "#27ae60"
                    iconPreview.Kirigami.Theme.neutralTextColor = "#f67400"
                    iconPreview.Kirigami.Theme.negativeTextColor = "#da4453"
                } else if (type == "dark") {
                    iconPreview.Kirigami.Theme.inherit = false
                    iconPreview.Kirigami.Theme.textColor = "#eff0f1"
                    iconPreview.Kirigami.Theme.backgroundColor = "#31363b"
                    iconPreview.Kirigami.Theme.highlightColor = "#3daee9"
                    iconPreview.Kirigami.Theme.highlightedTextColor = "#eff0f1"
                    iconPreview.Kirigami.Theme.positiveTextColor = "#27ae60"
                    iconPreview.Kirigami.Theme.neutralTextColor = "#f67400"
                    iconPreview.Kirigami.Theme.negativeTextColor = "#da4453"
                } else if (type == "active") {
                    iconPreview.Kirigami.Theme.inherit = true
                }
                ssPicker.open()
            }
        }
        Repeater {
            model: cuttlefish.actions
            delegate: QQC2.Button {
                Layout.alignment: Qt.AlignHCenter
                action: modelData
            }
        }
        Item {
            Layout.fillHeight: true
        }
        DualMontage { id: dualMont }
    }

    Kirigami.Separator {
        width: 1
        anchors {
            left: parent.left
            bottom: parent.bottom
            top: parent.top
        }
    }
}
