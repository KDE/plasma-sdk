/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
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

import QtQuick 2.1
// import QtQuick.Controls 1.0
// import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
//import QtQuick.Window 2.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents


Item {

    width: units.gridUnit * 40
    height: Math.round(width / 3 * 2)

    id: cuttlefish
    objectName: "cuttlefish"

    Component.onCompleted: {
        print("Hi from Cuttlefish!");
        //dirModel.rootIndex = dirModel.indexForUrl("file:///usr/share/icons")
        //dirModel.rootIndex = dirModel.index("/usr/share/icons")
    }

    PlasmaCore.IconItem {
        anchors.right: parent.right
        source: "plasma"
    }
    /*
    ListView {
        id: view
        anchors.fill: parent
        opacity: 0.2
        Text {
            anchors.bottom: parent.bottom
            //text: "count: " + dirModel}
        }

        model: VisualDataModel {
            model: iconModel

            delegate: Rectangle {
                width: 200; height: 25
                //Text { text: filePath }
                Text { text: "File: " + iconName }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (model.hasModelChildren)
                            view.model.rootIndex = view.model.modelIndex(index)
                    }
                }
            }
        }
    }
    */
    property int iconSize: units.iconSizes.large

    GridLayout {

        columns: 2
        anchors.fill: parent

        PlasmaComponents.ToolBar {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredHeight: units.gridUnit * 2
            tools: Row {
                PlasmaComponents.ToolButton {
                    iconSource: "go-previous"
                    onClicked: {


                    }
                }
                PlasmaComponents.TextField {
                    id: filterInput

                    Layout.fillWidth: true
                    anchors.verticalCenter: parent.verticalCenter

                    //text: currentWebView.url

                    onTextChanged: iconModel.filter = text
                    //Keys.onReturnPressed: load(browserManager.urlFromUserInput(text))
                }
            }
        }

        GridView {
            id: iconGrid

            focus: true
            //spacing: 0
            //rowSpacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

    //         cellWidth: iconSize
    //         cellHeight: iconSize

            model: iconModel

            highlight: Rectangle { color: theme.highlightColor }

            delegate: MouseArea {
                id: delegateRoot
                width: iconSize
                height: iconSize + units.gridUnit
                hoverEnabled: true

                function setAsPreview() {
                    print("preview() " + iconName + " " + fullPath);
                    preview.fullPath = fullPath
                    preview.iconName = iconName

                }

                PlasmaCore.IconItem {
                    width: iconSize
                    source: iconName
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                        bottom: parent.bottom
                    }
                }
                Connections {
                    target: iconGrid
                    onCurrentIndexChanged: {
                        if (delegateRoot.GridView.isCurrentItem) {
                            print("index changed" + iconName + " " + fullPath)
                            //preview.fullPath = fullPath
                            delegateRoot.setAsPreview();
                        }
                    }
                }
                onClicked: {
                    iconGrid.currentIndex = index
                }
                onEntered: {
                    setAsPreview();
                }
            }
        }
        Preview {
            id: preview
            Layout.preferredWidth: units.gridUnit * 12
            Layout.fillHeight: true
        }
    }
}
