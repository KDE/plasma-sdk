/*
 *   Copyright 2013 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents


Item {
    id: root
    signal formFactor(int FormFactorType)

    RowLayout {
        id: buttonRow
        anchors.fill: parent
        spacing: 0

        PlasmaComponents.Button {
            id: refreshButton
            iconSource: "view-refresh"
        }
        PlasmaComponents.Button {
            id: konsoleButton
            iconSource: "utilities-terminal"
        }
        PlasmaComponents.Button {
            id: contextMenuButton
            text: i18n("FormFactors")
            onClicked: formFactorMenu.open()
        }

        PlasmaComponents.ContextMenu {
            id: formFactorMenu
            PlasmaComponents.MenuItem {
                text: i18n("Planar")
                onClicked: formFactor(PlasmaCore.Types.Planar)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Vertical")
                onClicked: formFactor(PlasmaCore.Types.Vertical)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Horizontal")
                onClicked: formFactor(PlasmaCore.Types.Horizontal)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Mediacenter")
                onClicked: formFactor(PlasmaCore.Types.MediaCenter)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Application")
                onClicked: formFactor(PlasmaCore.Types.Application)
            }
        }

        PlasmaComponents.Button {
            id: locationMenuButton
            text: i18n("Location")
            onClicked: locationMenu.open()
        }

        PlasmaComponents.ContextMenu {
            id: locationMenu
            PlasmaComponents.MenuItem {
                text: i18n("Floating")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Desktop")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Fullscreen")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Topedge")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Bottomedget")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Leftedget")
            }
            PlasmaComponents.MenuItem {
                text: i18n("Rightedget")
            }
        }
    }
}
