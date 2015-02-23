/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.2
// import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras


GridView {
    id: iconGrid

    focus: true

    cellWidth: iconSize + units.gridUnit
    cellHeight: cellWidth + Math.round(units.gridUnit * 2)

    cacheBuffer: 10000
    highlightMoveDuration: 0
    boundsBehavior: Flickable.StopAtBounds
    model: iconModel

    highlight: Rectangle {
        color: theme.highlightColor
        //height: parent.height + units.gridUnit * 3
        anchors.bottomMargin: -units.gridUnit * 2
    }

    delegate: IconGridDelegate {}

    PlasmaComponents.BusyIndicator {
        running: iconModel.loading
        visible: running
        anchors.centerIn: parent
        width: units.gridUnit * 8
        height: width
    }
}
