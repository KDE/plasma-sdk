import QtQuick 1.0
import org.kde.qtextracomponents 0.1
import org.kde.plasma.graphicswidgets 0.1

Rectangle {
    id: page
    width: 500; height: 500
    color: "white"

    Component {
        id: appDelegate

        Item {
            width: 200; height: 100

            Rectangle {
                id: background
                width: parent.width - 2
                height: parent.height
                anchors.centerIn: parent
                radius: 5
                color: "lightsteelblue"
                opacity: 0
                Behavior on opacity {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.InOutCubic
                    }
                }
            }
            QIconItem {
                id: iconItem
                width: 64; height: 64
                y: 10; anchors.horizontalCenter: parent.horizontalCenter
                icon: decoration
            }
            Text {
                anchors { top: iconItem.bottom; horizontalCenter: parent.horizontalCenter }
                text: display
                width: parent.width - 8
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    var point = mapToItem(gridView, mouse.x, mouse.y)
                    var index = gridView.indexAt(point.x, point.y)
                    gridView.currentIndex = index

                    var modelIndex = filterModel.modelIndex(index)
                    if (hasModelChildren) {
                        filterModel.rootIndex = modelIndex
                    } else {
                        plasmaModel.runIndex(modelIndex)
                    }
                }
                onEntered: background.opacity = 0.5
                onExited: background.opacity = 0
            }
        }
    }

    Component {
        id: appHighlight
        Rectangle {
            radius: 5
            border.color: "black"
            border.width: 2
        }
    }

    Component {
        id: backBar

        Rectangle {
            height: backButton.height + 8
            width: parent.width
            color: "transparent"

            PushButton {
                id: backButton
                anchors.verticalCenter: parent.verticalCenter
                x: 4
                text: i18n("Back")
                onClicked: {
                    filterModel.rootIndex = filterModel.parentModelIndex
                }
            }
        }
    }

    VisualDataModel {
        id: filterModel
        model: plasmaModel
        delegate: appDelegate
    }

    GridView {
        id: gridView
        anchors.fill: parent
        cellWidth: 206; cellHeight: 106
        header: backBar
        highlight: appHighlight
        focus: true
        model: filterModel
    }
}
