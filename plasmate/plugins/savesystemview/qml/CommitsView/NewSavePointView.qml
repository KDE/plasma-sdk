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
import QtQuick.Controls 1.2 as QtControls
import QtQuick.Layouts 1.1 as QtLayouts

QtControls.ScrollView {
    QtLayouts.ColumnLayout {
        QtControls.Label {
            text: "Please enter a commit message"
        }

        QtControls.TextArea {
            id: textArea
        }

        QtLayouts.RowLayout {
            QtControls.Button {
                id: button
                text: "Commit"
                enabled: textArea.text != ""
                onClicked: {
                    git.newSavePoint(textArea.text)
                    stackView.pop()
                }
            }

            QtControls.Button {
                text: "Cancel"
                onClicked: {
                    stackView.pop()
                }
            }
        }
    }
}
