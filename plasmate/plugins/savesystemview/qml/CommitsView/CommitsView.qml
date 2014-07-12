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
    Flickable {
        id: flick
        Item {
            id: item
            anchors.fill: parent
            QtLayouts.RowLayout {
                id: row
                spacing: 10
                width: branch.width + newSavePoint.width + repeater.delegatesWidth +
                       (repeater.spacing * repeater.count) + (row.spacing * 3)
                height: item.height

                Branch {
                    id: branch
                    text: git.currentBranch
                }

                NewSavePoint {
                    id: newSavePoint
                }

                QtLayouts.RowLayout {
                    height: item.height
                    width: item.width - branch.width - newSavePoint.width
                    Repeater {
                        id: repeater
                        anchors.fill: parent
                        property int delegatesWidth: 0
                        model: git.commitsModel
                        Commit {
                            id: commit
                            text: message
                            commitAuthor: author
                            commitdate: CommitDate
                            Component.onCompleted: {
                                repeater.delegatesWidth += commit.width
                            }
                        }
                    }
                }
            } //end of row
        } // end of item
    }
}
