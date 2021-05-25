/*
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.5
import Qt.labs.platform 1.0

MenuBar {
    id: root

    signal quit()
    signal zoomIn()
    signal zoomOut()
    signal montage(int type)

    Menu {
        title: i18n("File")
        Menu {
            title: i18n("Export Montage with Color Scheme…")
            MenuItem {
                text: i18n("Active Color Scheme")
                onTriggered: root.montage(Screenshot.MontageType.Active)
            }
            MenuItem {
                text: i18n("Breeze (Normal)")
                onTriggered: root.montage(Screenshot.MontageType.Normal)
            }
            MenuItem {
                text: i18n("Breeze Dark")
                onTriggered: root.montage(Screenshot.MontageType.Dark)
            }
            MenuItem {
                text: i18n("Breeze (Normal) and Breeze Dark")
                onTriggered: root.montage(Screenshot.MontageType.Dual)
            }
        }
        MenuSeparator {}
        MenuItem {
            iconName: "gtk-quit"
            text: i18n("Quit")
            shortcut: StandardKey.Quit
            onTriggered: root.quit()
        }
    }
    Menu {
        title: i18n("View")
        MenuItem {
            iconName: "zoom-in"
            text: i18n("Zoom In")
            shortcut: StandardKey.ZoomIn
            onTriggered: root.zoomIn()
        }
        MenuItem {
            iconName: "zoom-out"
            text: i18n("Zoom Out")
            shortcut: StandardKey.ZoomOut
            onTriggered: root.zoomOut()
        }
    }
}
