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

QtLayouts.ColumnLayout {
    id: selectionBranchView
    property alias buttonText: button.text
    property alias title: label.text
    property alias input: comboBox.currentText

    signal doWork

    QtControls.Label {
        id: label
    }

    QtControls.ComboBox {
        id: comboBox
        //model: git.branches
        currentIndex: -1
    }

    QtLayouts.RowLayout {
        QtControls.Button {
            id: button
            enabled: currentIndex != -1
            onClicked: {
                selectionBranchView.doWork()
            }
        }

        QtControls.Button {
            text: "Cancel"
            onClicked: {
                stackView.pop()
            }
        }
    }

    Component.onCompleted: {
        var tmp = git.branches
        var currentBranch = git.currentBranch
        var branchList = []

        for(var i in tmp) {
            if (tmp[i] != currentBranch) {
                branchList.append(tmp[i])
            }
        }

        comboBox.model = branchList
    }
}

