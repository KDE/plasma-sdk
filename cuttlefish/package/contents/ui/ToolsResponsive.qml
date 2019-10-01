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

Rectangle {
    id: root 
    width: parent.width
    color: Kirigami.Theme.backgroundColor

    Kirigami.Theme.textColor: cuttlefish.textcolor
    Kirigami.Theme.backgroundColor: cuttlefish.bgcolor
    Kirigami.Theme.highlightColor: cuttlefish.highlightcolor
    Kirigami.Theme.highlightedTextColor: cuttlefish.highlightedtextcolor
    Kirigami.Theme.positiveTextColor: cuttlefish.positivetextcolor
    Kirigami.Theme.neutralTextColor: cuttlefish.neutraltextcolor
    Kirigami.Theme.negativeTextColor: cuttlefish.negativetextcolor

    property alias currentIndex: colorcombo.currentIndex
    property alias value: sizeslider.value

    signal colorschemeChanged(int index)
    signal sliderValueChanged(int val)

    Kirigami.Separator {
        height: 1
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.smallSpacing
        anchors.rightMargin: Kirigami.Units.smallSpacing
        anchors.verticalCenter: parent.verticalCenter
        spacing: Kirigami.Units.largeSpacing
        QQC2.Slider {
            visible: !cuttlefish.widescreen
            id: sizeslider
            Layout.fillWidth: true

            to: 6.0
            stepSize: 1.0
            snapMode: QQC2.Slider.SnapAlways
            value: 4.0

            onValueChanged: {
                sizetimer.restart()
                pixelSizeInput.text = indexToSize(sizeslider.value)
                root.sliderValueChanged(sizeslider.value)
            }

            Timer {
                id: sizetimer
                running: false
                repeat: false
                interval: 200
                onTriggered: iconSize = indexToSize(sizeslider.value)
            }

            Component.onCompleted: {
                pixelSizeInput.text = indexToSize(sizeslider.value);
            }
        }

        QQC2.Label {
            visible: !cuttlefish.widescreen
            id: pixelSizeInput

            Layout.preferredWidth: units.gridUnit * 1
        }
        QQC2.ComboBox {
            id: colorcombo
            visible: !cuttlefish.widescreen
            model: ["System Color Scheme", "Breeze (Normal)", "Breeze Dark"]
            delegate: QQC2.ItemDelegate {
                text: i18n(modelData)
                width: parent.width
            }
            onActivated: (index) => {
                root.colorschemeChanged(index)
            }
            popup.modal: false
        }
        Item {
            width: Kirigami.Units.largeSpacing * 3
        }
    }
}