/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

import QtQuick 2.3
import org.kde.plasma.core 2.0 as PlasmaCore
import QtQuick.Layouts 1.1 as QtLayouts
import QtQuick.Controls 1.2 as QtControls

SaveSystemItem {
    property string commitAuthor
    property date commitdate

    icon: "dialog-ok"

    PlasmaCore.ToolTipArea {
        id: toolTip
        width: parent.width * 2
        height: parent.height * 2
        mainItem: QtLayouts.ColumnLayout {
            QtControls.Label {
                text: i18n("Author: ") + commitAuthor
            }

            QtControls.Label {
                text: i18n("Date: ") + commitdate
            }
        }
    }
}

