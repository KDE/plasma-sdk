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
    width: 500
    height: 400
    visible: true

    menuBar: MenuBar {
        Menu {
            title: i18n("Menu")
            MenuItem {
                text: i18n("item1")
            }
            MenuItem {
                text: i18n("item1")
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            TextField {
                placeholderText: i18n("Search...")
                onTextChanged: searchModel.filterRegExp = ".*" + text + ".*"
            }
        }
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

        delegate: Loader {
            width: view.cellWidth
            height: view.cellHeight
            source: Qt.resolvedUrl("delegates/" + model.delegate + ".qml")
        }
    }

}
