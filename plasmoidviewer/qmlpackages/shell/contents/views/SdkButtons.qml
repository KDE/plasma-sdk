/*
 *   SPDX-FileCopyrightText: 2013 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root
    signal formFactor(int FormFactorType)
    signal location(int LocationType)
    signal requestScreenshot()
    property int backgroundHeight: refreshButton.implicitHeight * 2
    property int backgroundWidth: (refreshButton.implicitWidth + konsoleButton.implicitWidth +
                                  formFactorMenuButton.implicitWidth + locationMenuButton.implicitWidth) * 2

    RowLayout {
        id: buttonRow
        anchors.fill: parent

        PlasmaComponents.Button {
            id: refreshButton
            icon.name: "view-refresh"
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
            icon.name: "utilities-terminal"
            visible: desktop.konsoleVisible
            onClicked: konsolePreviewer.visible = !konsolePreviewer.visible
        }
        PlasmaComponents.Button {
            id: formFactorMenuButton
            text: i18n("FormFactors")
            onClicked: formFactorMenu.open()
        }

        PlasmaExtras.Menu {
            id: formFactorMenu
            visualParent: formFactorMenuButton
            PlasmaExtras.MenuItem {
                text: i18n("Planar")
                onClicked: formFactor(PlasmaCore.Types.Planar)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Vertical")
                onClicked: formFactor(PlasmaCore.Types.Vertical)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Horizontal")
                onClicked: formFactor(PlasmaCore.Types.Horizontal)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Mediacenter")
                onClicked: formFactor(PlasmaCore.Types.MediaCenter)
            }
            PlasmaExtras.MenuItem {
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
            icon.name: "ksnapshot"
            onClicked: requestScreenshot()
        }

        PlasmaExtras.Menu {
            id: locationMenu
            visualParent: locationMenuButton
            PlasmaExtras.MenuItem {
                text: i18n("Floating")
                onClicked: location(PlasmaCore.Types.Floating)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Desktop")
                onClicked: location(PlasmaCore.Types.Desktop)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Fullscreen")
                onClicked: location(PlasmaCore.Types.FullScreen)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Top Edge")
                onClicked: location(PlasmaCore.Types.TopEdge)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Bottom Edge")
                onClicked: location(PlasmaCore.Types.BottomEdge)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Left Edge")
                onClicked: location(PlasmaCore.Types.LeftEdge)
            }
            PlasmaExtras.MenuItem {
                text: i18n("Right Edge")
                onClicked: location(PlasmaCore.Types.RightEdge)
            }
        }

        PlasmaComponents.Button {
            id: configButton
            icon.name: "configure"
            onClicked: {
                var applet = containment.applets[0];
                if (applet) {
                    var action = applet.action('configure');
                    if (action) {
                        action.trigger();
                    }
                }
            }
        }
        PlasmaComponents.Button {
            text: i18n("Configure Containment")
            onClicked: {
                var action = containment.action('configure');
                if (action) {
                    action.trigger();
                }
            }
        }
        PlasmaComponents.Button {
            icon.name: "hide_table_row"
            onClicked: {
                root.visible = false;
            }
        }
    }
}
