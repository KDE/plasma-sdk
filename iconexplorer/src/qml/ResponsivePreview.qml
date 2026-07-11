/*
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.GlobalDrawer {
    id: root
    edge: Qt.RightEdge

    handleOpenIcon.source: "dialog-close"
    handleClosedIcon.source: "view-preview"

    focus: false

    actions: cuttlefish.actions

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
    IconMontage {
        id: grid
        columns: cuttlefish.iconSizes.length - 1
        Layout.alignment: Qt.AlignHCenter
    }
}
