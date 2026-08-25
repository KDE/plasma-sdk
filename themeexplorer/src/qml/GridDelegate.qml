/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */


import QtQuick

import org.kde.kirigami as Kirigami

Item {
    id: root

    required property int index
    required property bool usesFallback
    required property string delegate
    required property string imagePath
    required property string description
    required property string frameSvgPrefixes
    required property list<string> iconElements

    property bool showMargins

    width: GridView.view.cellWidth
    height: GridView.view.cellHeight
    MouseArea {
        z: 2
        anchors.fill: parent
        onClicked: root.GridView.view.currentIndex = root.index
    }
    Loader {
        z: -1
        anchors.fill: parent
        source: Qt.resolvedUrl("delegates/" + root.delegate + ".qml")
        onLoaded: {
            if ("showMargins" in item) {
                item.showMargins = Qt.binding(function() {
                    return root.showMargins
                });
            }
            if ("imagePath" in item) {
                item.imagePath = root.imagePath;
            }
            if ("frameSvgPrefixes" in item) {
                item.frameSvgPrefixes = root.frameSvgPrefixes
            }
            if ("iconElements" in item) {
                item.iconElements = root.iconElements
            }
        }
    }
    Rectangle {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Kirigami.Units.gridUnit
        }
        width: Kirigami.Units.gridUnit
        height: Kirigami.Units.gridUnit
        radius: Kirigami.Units.gridUnit
        opacity: 0.5
        color: root.usesFallback ? "red" : "green"
    }
}
