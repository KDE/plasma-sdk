/*
 *   SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.kde.kirigami 2.3 as Kirigami
import org.kde.plasma.sdk 1.0 as SDK

TextField {
    id: field
    property string key
    property string label
    Kirigami.FormData.label: label
    text: SDK.GlobalTheme[key]
    Layout.minimumWidth: Kirigami.Units.gridUnit * 15
    onAccepted: {
        SDK.GlobalTheme.name = text;
        timer.running = false;
    }
    onTextChanged: timer.restart()
    Timer {
        id: timer
        interval: 1000
        onTriggered: SDK.GlobalTheme[key] = field.text
    }
}
