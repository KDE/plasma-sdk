/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Carson Black <uhhadd@gmail.com>                        *
 *   Copyright 2019 David Redondo <kde@david-redondo.de>                   *
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
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.8 as Kirigami

Item {
    property QQC2.Action screenshotAction:  QQC2.Action {
        enabled: !iconPreview.screenshotting
        icon.name: "camera-web-symbolic"
        text: i18n("Create screenshot of icon")
        onTriggered: {
            screenshotPopup.popup()
        }
    }
    property list<Kirigami.Action> actions:[
        Kirigami.Action {
            text: i18n("Open icon with external program")
            iconName: "document-open"
            onTriggered: Qt.openUrlExternally(preview.fullPath)
        },
        Kirigami.Action {
            text: pickerMode ? i18n("Insert icon name") : i18n("Copy icon name to clipboard")
            iconName: "edit-copy"
            onTriggered: {
                clipboard(preview.iconName)
                cuttlefish.showPassiveNotification(i18n("Icon name copied to clipboard"), "short")
            }
        },
        Kirigami.Action {
            id: screenshotAction
            iconName: "camera-web-symbolic"
            text: i18n("Create screenshot of icon with...")
            onTriggered: screenshotPopup.popup()
            Kirigami.Action {
                text: i18n("Breeze Colors")
                onTriggered: previewPane.iconPreview.shot("normal")
            }
            Kirigami.Action {
                text: i18n("Breeze Dark Colors")
                onTriggered: previewPane.iconPreview.shot("dark")
            }
            Kirigami.Action {
                onTriggered: previewPane.dualMont.shot()
                text: i18n("Breeze (Normal) and Breeze Dark")
            }
            Kirigami.Action {
                onTriggered: previewPane.iconPreview.shot("active")
                text: i18n("Active Color Scheme")
            }
        },
        Kirigami.Action {
            text: i18n("View icon in other themes")
            icon.name: "document-equal"
            onTriggered: comparison.sheetOpen = true
        }
    ]

    QQC2.Menu {
        id: screenshotPopup
        Repeater {
            model: screenshotAction.visibleChildren
            delegate: QQC2.MenuItem {
                action: modelData
            }
        }
    }

    function clipboard(text) {
        if (!pickerMode) {
            clipboardHelper.text = text;
            clipboardHelper.selectAll();
            clipboardHelper.copy();
        } else {
            iconModel.output(text);
        }
    }
    TextEdit {
        id: clipboardHelper
        visible: false
    }

}
