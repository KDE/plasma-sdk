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

import QtQuick 2.5
import Qt.labs.platform 1.0

Menu {
    id: root
    Connections {
        target: cuttlefish
        onItemRightClicked: {
            root.open()
        }
    }
    MenuItem {
        iconName: "edit-copy"
        text: pickerMode ? i18n("Insert icon name") : i18n("Copy icon name to clipboard")
        onTriggered: {
            previewPane.clipboard(preview.iconName)
            cuttlefish.showPassiveNotification(i18n("Icon name copied to clipboard"), "short")
        }
    }
    MenuItem {
        iconName: "document-open"
        text: i18n("Open icon with external program")
        onTriggered: {
            Qt.openUrlExternally(preview.fullPath)
        }
    }
}
