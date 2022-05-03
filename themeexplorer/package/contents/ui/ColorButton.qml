/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore

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
        height: theme.mSize(theme.defaultFont).height
    }
}
