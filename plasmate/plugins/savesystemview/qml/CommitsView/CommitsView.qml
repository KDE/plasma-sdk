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
    id: scrollView
    height: parent.height/2
    width: parent.width

    QtLayouts.RowLayout {
        id: row
        spacing: 10
        Branch {
            id: branch
            text: git.currentBranch
        }

        NewSavePoint {
            id: newSavePoint
        }

        Component {
            id: delegate
            Commit {
                text: message
                commitAuthor: author
                commitdate: CommitDate
                Component.onCompleted: {
                    console.log(model.message)
                }
            }
        }

        ListView {
            id: listView
            width: scrollView.width - branch.width - newSavePoint.width
            height: branch.height
            spacing: 10
            orientation: ListView.Horizontal
            model: git.commitsModel
            delegate: delegate
        }
    }
}

