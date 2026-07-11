/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

GridView {
    id: iconGrid

    activeFocusOnTab: true
    keyNavigationEnabled: true

    topMargin: Kirigami.Units.gridUnit
    leftMargin: Kirigami.Units.gridUnit
    rightMargin: Kirigami.Units.gridUnit
    bottomMargin: Kirigami.Units.gridUnit

    cellWidth: iconSize + Math.round(Kirigami.Units.gridUnit * 1.5)
    cellHeight: iconSize + Math.round(Kirigami.Units.gridUnit * 3.5)

    cacheBuffer: 20
    highlightMoveDuration: 0
    model: proxyModel
    currentIndex: proxyModel.currentIndex

    highlight: Item {}

    delegate: IconGridDelegate {}

    QQC2.BusyIndicator {
        running: iconModel.loading
        visible: running
        anchors.centerIn: parent
        width: Kirigami.Units.gridUnit * 8
        height: width
    }
    Component.onCompleted: {
        currentItem.setAsPreview()
    }
}
