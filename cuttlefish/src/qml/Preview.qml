/*
    SPDX-FileCopyrightText: 2014-2017 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.2
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs

import org.kde.kirigami 2.20 as Kirigami

Rectangle {
    property alias iconPreview: iconPreview

    color: Kirigami.Theme.backgroundColor

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        Kirigami.SelectableLabel {
            Accessible.role: Accessible.Heading
            Layout.fillWidth: true

            Layout.topMargin: Kirigami.Units.gridUnit * 2

            // Match `level: 1` in Kirigami.Heading
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.35
            // Match `type: Kirigami.Heading.Type.Primary` in Kirigami.Heading
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
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
            columns: cuttlefish.iconSizes.length - 1
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing * 4
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
