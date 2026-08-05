/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Button {
    id: colorButton
    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
    onClicked: {
        colorDialog.activeButton = colorButton;
        colorDialog.open();
    }
    property alias color: colorRect.color;

    Rectangle {
        id: colorRect
        anchors.centerIn: parent
        width: parent.width - 2 * Kirigami.Units.smallSpacing
        height: Kirigami.Units.gridUnit
    }
}
