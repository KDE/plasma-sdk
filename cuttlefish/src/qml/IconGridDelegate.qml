/*
    SPDX-FileCopyrightText: 2014-2017 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.8 as Kirigami
// for smallestFont
import org.kde.plasma.core 2.0 as PlasmaCore


MouseArea {
    id: delegateRoot

    width: GridView.view.cellWidth
    height: GridView.view.cellHeight

    acceptedButtons: Qt.LeftButton | Qt.RightButton

    function setAsPreview() {
        preview.fullPath = fullPath
        preview.iconName = iconName
        preview.fileName = fileName
        preview.category = category
        preview.type = type
        preview.iconTheme = iconTheme
        preview.sizes = sizes
        preview.scalable = scalable;
    }

    Rectangle {
        color: Kirigami.Theme.highlightColor
        opacity: iconGrid.currentIndex == index ? 0.5 : 0.0
        visible: opacity != 0.0
        anchors.fill: parent
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration } }
    }

    Kirigami.Icon {
        id: delegateIcon
        width: iconSize
        height: iconSize
        source: iconName
        anchors {
            top: parent.top
            topMargin: Kirigami.Units.smallSpacing
            horizontalCenter: parent.horizontalCenter
        }
    }

    QQC2.Label {
        font.pointSize: iconSize > 96 ? Kirigami.Theme.defaultFont.pointSize : theme.smallestFont.pointSize
        text: iconName
        wrapMode: Text.Wrap
        maximumLineCount: 3
        horizontalAlignment: Text.AlignHCenter
        opacity: iconGrid.currentIndex == index ? 1.0 : 0.7
        anchors {
            top: delegateIcon.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: Kirigami.Units.smallSpacing
        }
    }

    Connections {
        target: iconGrid
        function onCurrentItemChanged() {
            if (delegateRoot.GridView.isCurrentItem) {
                delegateRoot.setAsPreview();
            }
        }
    }
    onClicked: (mouse) => {
        proxyModel.currentIndex = index
        iconGrid.forceActiveFocus();
        if (mouse.button == Qt.RightButton) {
            cuttlefish.itemRightClicked()
        }
    }
}
