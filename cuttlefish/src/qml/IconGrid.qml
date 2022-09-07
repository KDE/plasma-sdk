/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.8 as Kirigami

GridView {
    id: iconGrid
    focus: true

    cellWidth: iconSize + Math.round(Kirigami.Units.gridUnit * 1.5)
    cellHeight: cellWidth + Math.round(Kirigami.Units.gridUnit * 2)

    cacheBuffer: 20
    highlightMoveDuration: 0
    boundsBehavior: Flickable.StopAtBounds
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
