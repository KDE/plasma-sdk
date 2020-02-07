import QtQuick 2.5
import Qt.labs.platform 1.0

/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Carson Black <uhhadd@gmail.com>                        *
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

MenuBar {
    id: root

    signal quit()
    signal zoomIn()
    signal zoomOut()
    signal montage(int type)

    Menu {
        title: i18n("File")
        Menu {
            title: i18n("Export Montage with Color Scheme...")
            MenuItem {
                text: i18n("Active Color Scheme")
                onTriggered: root.montage(0)
            }
            MenuItem {
                text: i18n("Breeze (Normal)")
                onTriggered: root.montage(1)
            }
            MenuItem {
                text: i18n("Breeze Dark")
                onTriggered: root.montage(2)
            }
            MenuItem {
                text: i18n("Breeze (Normal) and Breeze Dark")
                onTriggered: root.montage(0)
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
