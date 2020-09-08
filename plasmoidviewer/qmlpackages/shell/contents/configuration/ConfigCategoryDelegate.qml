/*
 *  SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import org.kde.kquickcontrolsaddons 2.0

MouseArea {
    id: delegate

//BEGIN properties
    y: units.smallSpacing *2
    width: parent.width
    height: delegateContents.height + units.smallSpacing * 4
    hoverEnabled: true
    property bool current: (model.kcm && main.currentItem.kcm && model.kcm == main.currentItem.kcm) || (model.source == main.sourceFile)
    property string name: model.name
//END properties

//BEGIN functions
    function openCategory() {
        if (current) {
            return;
        }
        if (typeof(categories.currentItem) !== "undefined") {
            main.invertAnimations = (categories.currentItem.y > delegate.y);
            categories.currentItem = delegate;
        }
        if (model.source) {
            main.sourceFile = model.source;
        } else if (model.kcm) {
            main.sourceFile = "";
            main.sourceFile = Qt.resolvedUrl("ConfigurationKcmPage.qml");
            main.currentItem.kcm = model.kcm;
        } else {
            main.sourceFile = "";
        }
        main.title = model.name
    }
//END functions

//BEGIN connections
    onClicked: {
        //print("model source: " + model.source + " " + main.sourceFile);
        if (applyButton.enabled) {
            messageDialog.delegate = delegate;
            messageDialog.open();
            return;
        }
        if (delegate.current) {
            return;
        } else {
            openCategory();
        }
    }
    onCurrentChanged: {
        if (current) {
            categories.currentItem = delegate;
        }
    }
//END connections

//BEGIN UI components
    Rectangle {
        anchors.fill: parent
        color: syspal.highlight
        opacity: {
            if (categories.currentItem == delegate) {
                return 1
            } else if (delegate.containsMouse) {
                return 0.3 // there's no "hover" color in SystemPalette
            } else {
                return 0
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: units.longDuration
            }
        }
    }

    Column {
        id: delegateContents
        spacing: units.smallSpacing
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        QIconItem {
            anchors.horizontalCenter: parent.horizontalCenter
            width: units.iconSizes.medium
            height: width
            icon: model.icon
        }
        QtControls.Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: model.name
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            color: current ? syspal.highlightedText : syspal.text
            Behavior on color {
                ColorAnimation {
                    duration: units.longDuration
                    easing.type: "InOutQuad"
                }
            }
        }
    }
//END UI components
}
