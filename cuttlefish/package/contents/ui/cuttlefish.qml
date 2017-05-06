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

import QtQuick 2.1
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras


Item {

    width: units.gridUnit * 60
    height: Math.round(width / 3 * 2)

    property int iconSize: units.iconSizes.large
    property bool hoveredHighlight: false
    property bool darkScheme: false
    property bool usesPlasmaTheme: true

    id: cuttlefish
    objectName: "cuttlefish"
    state: "svgs"

    function indexToSize(ix) {

            var sizes = new Array();
            sizes[0] = units.iconSizes.tiny;
            sizes[1] = units.iconSizes.small;
            sizes[2] = units.iconSizes.smallMedium;
            sizes[3] = units.iconSizes.medium;
            sizes[4] = units.iconSizes.large;
            sizes[5] = units.iconSizes.huge;
            sizes[6] = units.iconSizes.enormous;

            return sizes[ix];
    }

    PlasmaCore.ColorScope {
        anchors.fill: parent
        colorGroup: darkScheme ? PlasmaCore.Theme.ComplementaryColorGroup : PlasmaCore.Theme.NormalColorGroup
        Rectangle {
            color: PlasmaCore.ColorScope.backgroundColor
            anchors.fill: parent
        }

        GridLayout {
            columns: 2
            anchors.fill: parent
            rowSpacing: - Math.round(units.gridUnit / 20)

            Tools {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.preferredHeight: units.gridUnit * 2
            }

            PlasmaExtras.ScrollArea {
                Layout.fillWidth: true
                Layout.fillHeight: true
                IconGrid {
                    id: iconGrid
                    anchors.fill: parent

                    footer: SvgGrid {
                        id: svgGrid
                        interactive: false
                    }
                }
            }
            Preview {
                id: preview
                Layout.preferredWidth: Math.max(parent.width / 4, units.gridUnit * 12)
                Layout.fillHeight: true
            }
        }
    }
    states: [
        State {
            name: "icons"
        },
        State {
            name: "svgs"
        }
    ]
}
