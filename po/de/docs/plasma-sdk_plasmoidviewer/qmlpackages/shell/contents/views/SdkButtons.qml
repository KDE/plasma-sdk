/*
 *   SPDX-FileCopyrightText: 2013 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
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
    property int backgroundHeight: refreshButton.implicitHeight * 2
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
            visible: desktop.konsoleVisible
            onClicked: konsolePreviewer.visible = !konsolePreviewer.visible
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
                text: i18n("Top Edge")
                onClicked: location(PlasmaCore.Types.TopEdge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Bottom Edge")
                onClicked: location(PlasmaCore.Types.BottomEdge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Left Edge")
                onClicked: location(PlasmaCore.Types.LeftEdge)
            }
            PlasmaComponents.MenuItem {
                text: i18n("Right Edge")
                onClicked: location(PlasmaCore.Types.RightEdge)
            }
        }

        PlasmaComponents.Button {
            id: configButton
            iconSource: "configure"
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
            iconSource: "hide_table_row"
            onClicked: {
                root.visible = false;
            }
        }
    }
}
