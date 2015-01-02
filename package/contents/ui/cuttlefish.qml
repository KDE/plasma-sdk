/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
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
// import QtQuick.Controls 1.0
// import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
//import QtQuick.Window 2.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras


Item {

    width: units.gridUnit * 60
    height: Math.round(width / 3 * 2)

    property int iconSize: units.iconSizes.large
    property bool hoveredHighlight: false

    id: cuttlefish
    objectName: "cuttlefish"
    state: "svgs"

    function indexToSize(ix) {

        /*
         * from kiconloader.h
         enum StdSizes {
            /// small icons for menu entries
            SizeSmall = 16,
            /// slightly larger small icons for toolbars, panels, etc
            SizeSmallMedium = 22,
            /// medium sized icons for the desktop
            SizeMedium = 32,
            /// large sized icons for the panel
            SizeLarge = 48,
            /// huge sized icons for iconviews
            SizeHuge = 64,
            /// enormous sized icons for iconviews
            SizeEnormous = 128

         * from Plasma's units.cpp (this corrects for DPI)
            m_iconSizes->insert("tiny", devicePixelIconSize(KIconLoader::SizeSmall) / 2);
            m_iconSizes->insert("small", devicePixelIconSize(KIconLoader::SizeSmall));
            m_iconSizes->insert("smallMedium", devicePixelIconSize(KIconLoader::SizeSmallMedium));
            m_iconSizes->insert("medium", devicePixelIconSize(KIconLoader::SizeMedium));
            m_iconSizes->insert("large", devicePixelIconSize(KIconLoader::SizeLarge));
            m_iconSizes->insert("huge", devicePixelIconSize(KIconLoader::SizeHuge));
            m_iconSizes->insert("enormous", devicePixelIconSize(KIconLoader::SizeEnormous));

            */
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

    Component.onCompleted: {
        print("Hi from Cuttlefish!");
        //dirModel.rootIndex = dirModel.indexForUrl("file:///usr/share/icons")
        //dirModel.rootIndex = dirModel.index("/usr/share/icons")
    }

    Rectangle {

        color: theme.backgroundColor
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
                    //anchors.fill: parent
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
    states: [
        State {
            name: "icons"
            //when: navigationShown
//             PropertyChanges { target: iconGrid; opacity: 1.0 }
//             PropertyChanges { target: svgGrid; opacity: 0.0 }
        },
        State {
            name: "svgs"
            //when: !navigationShown
//             PropertyChanges { target: iconGrid; opacity: 0.0 }
//             PropertyChanges { target: svgGrid; opacity: 1.0 }
        }
    ]

}
