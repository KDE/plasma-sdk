/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras


Rectangle {

    id: preview

    color: PlasmaCore.ColorScope.backgroundColor

//     Rectangle {
//         anchors.fill: parent
//         color: "orange"
//     }

    property string iconName: "plasma"
    property string fullPath: ""
    property string category: ""
    property string fileName: ""
    property string type: ""
    property string iconTheme: ""
    property variant sizes: []
    property bool scalable: true

    function clipboard(text) {
        if (!pickerMode) {
            clipboardHelper.text = text;
            clipboardHelper.selectAll();
            clipboardHelper.copy();
        } else {
            print("Clippie!" + text);
            iconModel.output(text);
        }

    }
    TextEdit {
        id: clipboardHelper
        visible: false
    }


    ColumnLayout {

        anchors.margins: units.gridUnit
        anchors.rightMargin: units.gridUnit * 2

        anchors.fill: parent

        spacing: units.gridUnit / 2

        Item { height: units.gridUnit / 2 }

        PlasmaExtras.Heading {
            level: 3
            elide: Text.ElideRight
            Layout.fillWidth: true
            Layout.fillHeight: false
            wrapMode: Text.Wrap
            text: iconName
        }
        RowLayout {
            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.preferredHeight: indexToSize(4)
            anchors.horizontalCenter: parent.horizontalCenter

            PlasmaCore.IconItem {
                source: iconName
                colorGroup: PlasmaCore.ColorScope.colorGroup
                Layout.preferredWidth: indexToSize(0)
                Layout.preferredHeight: indexToSize(0)
            }
            PlasmaCore.IconItem {
                source: iconName
                colorGroup: PlasmaCore.ColorScope.colorGroup
                Layout.preferredWidth: indexToSize(1)
                Layout.preferredHeight: indexToSize(1)
            }
            PlasmaCore.IconItem {
                source: iconName
                colorGroup: PlasmaCore.ColorScope.colorGroup
                Layout.preferredWidth: indexToSize(2)
                Layout.preferredHeight: indexToSize(2)
            }
            PlasmaCore.IconItem {
                source: iconName
                colorGroup: PlasmaCore.ColorScope.colorGroup
                Layout.preferredWidth: indexToSize(3)
                Layout.preferredHeight: indexToSize(3)
            }
            PlasmaCore.IconItem {
                source: iconName
                colorGroup: PlasmaCore.ColorScope.colorGroup
                Layout.preferredWidth: indexToSize(4)
                Layout.preferredHeight: indexToSize(4)
            }
        }
        /*
        PlasmaCore.IconItem {
            source: iconName
            Layout.fillWidth: true
            Layout.preferredWidth: indexToSize(5)
            Layout.preferredHeight: indexToSize(5)
        }
        PlasmaCore.IconItem {
            source: iconName
            Layout.fillHeight: false
            Layout.preferredWidth: indexToSize(6)
            Layout.preferredHeight: indexToSize(6)
        }
        */
        PlasmaCore.IconItem {
            source: iconName
            colorGroup: PlasmaCore.ColorScope.colorGroup
            Layout.fillHeight: false
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.width
        }
        /*
        RowLayout {

            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: indexToSize(4)


            Image {
                source: fullPath
                Layout.fillWidth: false
                Layout.preferredWidth: sourceSize.width
                Layout.preferredHeight: sourceSize.height
            }
        }
        */
        GridLayout {
            columns: 2

            PlasmaComponents.Label {
                text: i18n("Name:")
            }
            PlasmaComponents.Label {
                text: iconName
                wrapMode: Text.Wrap
            }
            PlasmaComponents.Label {
                text: i18n("Filename:")
            }
            PlasmaComponents.Label {
                text: fileName
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignVCenter
            }
            PlasmaComponents.Label {
                text: i18n("Category:")
            }
            PlasmaComponents.Label {
                text: category
                wrapMode: Text.WordWrap
            }
            PlasmaComponents.Label {
                text: i18n("Scalable:")
            }
            PlasmaComponents.Label {
                text: {
//                     print("SS:" + sizes);
                    /*var out;
                    for (s in sizes) {
                        out = out + ", " + s;
                    }
                    return out;*/
                    if (scalable) {
                        return i18n("yes");
                    } else {
                        return i18n("no");
                    }
                }
            }
            PlasmaComponents.Label {
                text: i18n("Sizes:")
            }
            PlasmaComponents.Label {
                text: (sizes != undefined) ? sizes.join(", ") : ""
//                 text: {
//                     print("SS:" + sizes);
//                     var out;
//                     for (s in sizes) {
//                         out = out + ", " + s;
//                     }
//                     return out;
//                 }
            }
        }
        Item {
            Layout.fillHeight: true
        }
        PlasmaComponents.ToolButton {
            text: pickerMode ? i18n("Insert icon name") : i18n("Copy icon name to clipboard")
            iconSource: "edit-copy"
            Layout.fillWidth: true
            onClicked: clipboard(preview.iconName);
        }
        PlasmaComponents.ToolButton {
            text: pickerMode ?  i18n("Insert QtQuick code") : i18n("Copy QtQuick code to clipboard")
            iconSource: "edit-copy"
            Layout.fillWidth: true
            onClicked: {
                var code = "/* Don't forget to import...\n\
import org.kde.plasma.core 2.0 as PlasmaCore\n\
*/\n\
    PlasmaCore.IconItem {\n\
        source: \"" + preview.iconName + "\"\n\
        Layout.preferredWidth: units.iconSizes.medium\n\
        Layout.preferredHeight: units.iconSizes.medium\n\
    }\n\
";
                clipboard(code);
            }
        }
        PlasmaComponents.CheckBox {
            text: i18n("Update preview on hover")
            onCheckedChanged: hoveredHighlight = checked
        }
    }

    Rectangle {
        color: theme.highlightColor
        width: Math.round(units.gridUnit / 20)
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }
}
