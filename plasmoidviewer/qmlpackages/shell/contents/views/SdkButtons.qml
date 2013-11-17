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
    signal location(int LocationType)
    signal requestScreenshot()
    property int  backgroundHeight: refreshButton.implicitHeight + konsoleButton.implicitHeight +
                                    formFactorMenuButton.implicitHeight + locationMenuButton.implicitHeight
    property int backgroundWidth: (refreshButton.implicitWidth + konsoleButton.implicitWidth +
                                  formFactorMenuButton.implicitWidth + locationMenuButton.implicitWidth) * 2

    RowLayout {
        id: buttonRow
        anchors.fill: parent

        PlasmaComponents.Button {
            id: refreshButton
            iconSource: "view-refresh"
            onClicked: {
                var applet = containment.applets[0];
                if (applet) {
                    var action = applet.action('remove');
                    if (action) {
                        action.trigger();
                    }
                }
            }
        }
        PlasmaComponents.Button {
            id: konsoleButton
            iconSource: "utilities-terminal"
        }
        PlasmaComponents.Button {
            id: formFactorMenuButton
            text: i18n("FormFactors")
            onClicked: formFactorMenu.open()
        }

        PlasmaComponents.ContextMenu {
            id: formFactorMenu
            visualParent: formFactorMenuButton
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

        PlasmaComponents.Button {
            id: screenshotButton
            iconSource: "ksnapshot"
            onClicked: requestScreenshot()
        }

        PlasmaComponents.ContextMenu {
            id: locationMenu
            visualParent: locationMenuButton
            PlasmaComponents.MenuItem {
                text: i18n("Floating")
                onClicked: location(PlasmaCore.Types.Floating)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Desktop")
                onClicked: location(PlasmaCore.Types.Desktop)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Fullscreen")
                onClicked: location(PlasmaCore.Types.FullScreen)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Topedge")
                onClicked: location(PlasmaCore.Types.TopEdge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Bottomedge")
                onClicked: location(PlasmaCore.Types.Bottomedge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Leftedge")
                onClicked: location(PlasmaCore.Types.LeftEdge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Rightedget")
                onClicked: location(PlasmaCore.Types.RightEdge)
            }
        }
    }
}
