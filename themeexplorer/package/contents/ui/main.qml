/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore

ApplicationWindow {
    id: root
    width: units.gridUnit * 50
    height: units.gridUnit * 35
    visible: true

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            TextField {
                placeholderText: i18n("Search...")
                onTextChanged: searchModel.filterRegExp = ".*" + text + ".*"
            }
        }
    }

    SystemPalette {
        id: palette
    }

    ScrollView {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: sidebar.left
        }
        GridView {
            id: view
            anchors.fill: parent
            model: PlasmaCore.SortFilterModel {
                id: searchModel
                sourceModel: themeModel
                filterRole: "imagePath"
            }
            cellWidth: units.gridUnit * 15
            cellHeight: cellWidth
            highlightMoveDuration: 0

            highlight: Rectangle {
                radius: 3
                color: palette.highlight
            }
            delegate: MouseArea {
                width: view.cellWidth
                height: view.cellHeight
                property QtObject modelData: model
                onClicked: {
                    view.currentIndex = index;
                }
                Loader {
                    anchors.fill: parent
                    source: Qt.resolvedUrl("delegates/" + model.delegate + ".qml")
                }
            }
        }
    }
    Item {
        id: sidebar
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        width: root.width / 3
        ColumnLayout {
            anchors {
                fill: parent
                margins: units.gridUnit
            }
            Loader {
                id: extendedLoader
                property QtObject model: view.currentItem.modelData
                Layout.fillWidth: true
                Layout.minimumHeight: width
                source: Qt.resolvedUrl("delegates/" + model.delegate + ".qml")
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Label {
                Layout.fillWidth: true
                text: i18n("Image path: %1", view.currentItem.modelData.imagePath)
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                text: i18n("Description: %1", view.currentItem.modelData.description)
                wrapMode: Text.WordWrap
            }
            Button {
                text: i18n("Open In Editor...")
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
