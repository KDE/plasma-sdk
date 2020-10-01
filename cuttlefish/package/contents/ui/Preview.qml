/*
    SPDX-FileCopyrightText: 2014-2017 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
        IconMontage {
            id: iconPreview

            property bool screenshotting: false

            columns: cuttlefish.iconSizes.length - 1
            showWatermark: iconPreview.screenshotting

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
            Layout.preferredHeight: implicitHeight + (screenshotting ? (Kirigami.Units.gridUnit * 4) : 0)

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing * 4

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
