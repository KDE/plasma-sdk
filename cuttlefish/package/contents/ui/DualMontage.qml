/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Carson Black <uhhadd@gmail.com>                        *
 *   Copyright 2020 David Redondo <kde@david-redondo.de>                   *
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

Item {
    id: dualMontage
    visible: false
    height: 512
    width: 512
    Kirigami.Theme.inherit: false
    function shot() {
        ssPicker.open()
    }
    FileDialog {
        id: ssPicker
        selectExisting: false
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            dualMontage.grabToImage(function(result) {
                res = result.saveToFile(ssPicker.fileUrl.toString().slice(7))
            });
        }
        nameFilters: [ "PNG screenshot files (*.png)" ]
    }
    Column {
        Rectangle {
            height: 256
            width: 512
            color: Kirigami.Theme.backgroundColor
            Kirigami.Theme.inherit: false
            Kirigami.Theme.textColor: "#232629"
            Kirigami.Theme.backgroundColor: "#eff0f1"
            Kirigami.Theme.highlightColor: "#3daee9"
            Kirigami.Theme.highlightedTextColor: "#eff0f1"
            Kirigami.Theme.positiveTextColor: "#27ae60"
            Kirigami.Theme.neutralTextColor: "#f67400"
            Kirigami.Theme.negativeTextColor: "#da4453"

            RowLayout {
                id: previewGrid
                anchors.centerIn: parent
                Repeater {
                    model: cuttlefish.iconSizes
                    delegate: ColumnLayout {
                        Layout.alignment: Qt.AlignBottom
                        Kirigami.Icon {
                            source: preview.iconName
                            width: modelData
                            height: width
                        }
                        QQC2.Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: modelData
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
            }
        }
        Rectangle {
            height: 256
            width: 512
            color: Kirigami.Theme.backgroundColor
            Kirigami.Theme.inherit: false
            Kirigami.Theme.textColor: "#eff0f1"
            Kirigami.Theme.backgroundColor: "#31363b"
            Kirigami.Theme.highlightColor: "#3daee9"
            Kirigami.Theme.highlightedTextColor: "#eff0f1"
            Kirigami.Theme.positiveTextColor: "#27ae60"
            Kirigami.Theme.neutralTextColor: "#f67400"
            Kirigami.Theme.negativeTextColor: "#da4453"

            RowLayout {
                anchors.centerIn: parent
                Repeater {
                    model: cuttlefish.iconSizes
                    delegate: ColumnLayout {
                        Layout.alignment: Qt.AlignBottom
                        Kirigami.Icon {
                            source: preview.iconName
                            width: modelData
                            height: width
                        }
                        QQC2.Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: modelData
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
    }
}
