/*
 *   SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami

RowLayout {
    id: control

    property bool usesFallback

    property int /*Mode*/ mode: PresenceIndicator.Mode.Compact
    enum Mode {
        /** Only indicator with a tooltip. */
        Compact,
        /** Indicator and a label. */
        Full
    }

    spacing: Kirigami.Units.largeSpacing

    Rectangle {
        Layout.alignment: Qt.AlignBaseline

        implicitWidth: Kirigami.Units.gridUnit
        implicitHeight: Kirigami.Units.gridUnit
        radius: Kirigami.Units.gridUnit
        opacity: 0.5
        color: control.usesFallback ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.positiveTextColor
        border.color: Qt.darker(color)
        border.width: 1

        MouseArea {
            id: area
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.WhatsThisCursor
            acceptedButtons: Qt.NoButton
            visible: control.mode === PresenceIndicator.Mode.Compact

        }

        QQC2.ToolTip {
            text: label.text
            visible: area.containsMouse
        }
    }

    QQC2.Label {
        id: label
        // Layout.alignment: Qt.AlignBaseline
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: control.usesFallback ? i18n("Missing from this theme") : i18n("Present in this theme")
        visible: control.mode === PresenceIndicator.Mode.Full
    }
}
