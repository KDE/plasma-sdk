/*
 *  Copyright 2013 Marco Martin <mart@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */
import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    id: root
    objectName: "org.kde.desktop-CompactApplet"

    property Item fullRepresentation
    property Item compactRepresentation

    onFullRepresentationChanged: {

        //if the fullRepresentation size was restored to a stored size, or if is dragged from the desktop, restore popup size
        if (fullRepresentation.width > 0) {
            popupWindow.mainItem.width = fullRepresentation.width;
        } else if (fullRepresentation.Layout && fullRepresentation.Layout.preferredWidth > 0) {
            popupWindow.mainItem.width = fullRepresentation.Layout.preferredWidth
        } else if (fullRepresentation.implicitWidth > 0) {
            popupWindow.mainItem.width = fullRepresentation.implicitWidth
        } else {
            popupWindow.mainItem.width = theme.mSize(theme.defaultFont).width * 35
        }

        if (fullRepresentation.height > 0) {
            popupWindow.mainItem.height = fullRepresentation.height;
        } else if (fullRepresentation.Layout && fullRepresentation.Layout.preferredHeight > 0) {
            popupWindow.mainItem.height = fullRepresentation.Layout.preferredHeight
        } else if (fullRepresentation.implicitHeight > 0) {
            popupWindow.mainItem.height = fullRepresentation.implicitHeight
        } else {
            popupWindow.mainItem.width = theme.mSize(theme.defaultFont).height * 25
        }

        fullRepresentation.parent = appletParent;
        fullRepresentation.anchors.fill = fullRepresentation.parent;
    }

 
    PlasmaCore.Dialog {
        id: popupWindow
        objectName: "popupWindow"
        flags: Qt.WindowStaysOnTopHint
        visible: plasmoid.expanded
        visualParent: compactRepresentation ? compactRepresentation : null
        location: plasmoid.location
        hideOnWindowDeactivate: plasmoid.hideOnWindowDeactivate

        mainItem: Item {
            id: appletParent
            Layout.minimumWidth: (fullRepresentation && fullRepresentation.Layout) ? fullRepresentation.Layout.minimumWidth : 0
            Layout.minimumHeight: (fullRepresentation && fullRepresentation.Layout) ? fullRepresentation.Layout.minimumHeight: 0
        }

        onVisibleChanged: {
            if (!visible) {
                plasmoid.expanded = false
            } else {
                // This call currently fails and complains at runtime:
                // QWindow::setWindowState: QWindow::setWindowState does not accept Qt::WindowActive
                popupWindow.requestActivate();
            }
        }

    }
}
