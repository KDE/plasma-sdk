/*
 *   Copyright 2011-2013 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

import "plasmapackage:/code/LayoutManager.js" as LayoutManager

Item {
    id: appletHandle

    opacity: appletItem.controlsOpacity
    visible: opacity > 0
    width: appletItem.handleWidth
    height: appletItem.handleHeight
    //z: dragMouseArea.z + 1

    property int buttonMargin: 6
    property int minimumHeight:  6 * (root.iconSize + buttonMargin)
    property Item buttonsParent: appletHandle

    signal removeApplet

    function updateHeight() {
        var mini = appletHandle.minimumHeight + margins.top + margins.bottom;
        if (height > mini) {
            appletItem.handleMerged = true;
            height = appletItem.handleMerged ? appletItem.height : minimumHeight
            buttonsParent = appletHandle;
        } else if (appletItem.handleMerged) {
            appletItem.handleMerged = false;
            buttonsParent = noBackgroundHandle;
        }
    }

    PlasmaCore.FrameSvgItem {
        id: noBackgroundHandle

        width: handleWidth + margins.left + margins.right - 4
        height: (backgroundHints != "NoBackground") ? appletItem.handleHeight : appletItem.handleHeight + noBackgroundHandle.margins.top + noBackgroundHandle.margins.bottom
        visible: controlsOpacity > 0
        z: plasmoidBackground.z - 10

        anchors {
            verticalCenter: buttonsParent.verticalCenter
            left: buttonsParent.right
            leftMargin: handleMerged ? ((1-controlsOpacity) * appletItem.handleWidth) * -1 - appletItem.handleWidth * 2 + 2 : ((1-controlsOpacity) * appletItem.handleWidth) * -1 - appletItem.handleWidth
        }
        smooth: true
        imagePath: (backgroundHints == "NoBackground" || !handleMerged) ? "widgets/background" : ""
    }

    Column {
        id: buttonColumn
        width: handleWidth
        parent: buttonsParent
        anchors {
            top: parent.top
            topMargin: (!appletItem.hasBackground || !handleMerged) ? noBackgroundHandle.margins.top : appletItem.margins.top
            bottom: parent.bottom
            bottomMargin: (!appletItem.hasBackground && !handleMerged) ? noBackgroundHandle.margins.bottom : appletItem.margins.bottom
            right: parent.right
            rightMargin: appletItem.handleMerged ? -buttonMargin : noBackgroundHandle.margins.right - buttonMargin
        }
        spacing: buttonMargin*2
        ActionButton {
            svg: configIconsSvg
            elementId: "size-diagonal-tr2bl"
            iconSize: root.iconSize
            visible: (action && typeof(action) != "undefined") ? action.enabled : false
            action: (applet) ? applet.action("configure") : null
            Component.onCompleted: {
                if (action && typeof(action) != "undefined") {
                    action.enabled = true
                }
            }
        }
        ActionButton {
            id: rotateButton
            svg: configIconsSvg
            elementId: "rotate"
            iconSize: root.iconSize
            action: (applet) ? applet.action("rotate") : null
            Component.onCompleted: {
                if (action && typeof(action) != "undefined") {
                    action.enabled = true
                }
            }
            MouseArea {

                anchors {
                    fill: parent
                    margins: -buttonMargin
                }

                property int startRotation
                property real startCenterRelativeAngle;

                function pointAngle(pos) {
                    var r = Math.sqrt(pos.x * pos.x + pos.y * pos.y);
                    var cosine = pos.x / r;

                    if (pos.y >= 0) {
                        return Math.acos(cosine) * (180/Math.PI);
                    } else {
                        return -Math.acos(cosine) * (180/Math.PI);
                    }
                }

                function centerRelativePos(x, y) {
                    var mousePos = appletItem.parent.mapFromItem(rotateButton, x, y);
                    var centerPos = appletItem.parent.mapFromItem(appletItem, appletItem.width/2, appletItem.height/2);

                    mousePos.x -= centerPos.x;
                    mousePos.y -= centerPos.y;
                    return mousePos;
                }

                onPressed: {
                    mouse.accepted = true
                    animationsEnabled = false;
                    startRotation = appletItem.rotation;
                    LayoutManager.setSpaceAvailable(appletItem.x, appletItem.y, appletItem.width, appletItem.height, true)

                    startCenterRelativeAngle = pointAngle(centerRelativePos(mouse.x, mouse.y));
                }
                onPositionChanged: {

                    var rot = startRotation%360;
                    var snap = 4;
                    var newRotation = pointAngle(centerRelativePos(mouse.x, mouse.y)) - startCenterRelativeAngle + startRotation;

                    if (newRotation < 0) {
                        newRotation = newRotation + 360;
                    }

                    snapIt(0);
                    snapIt(90);
                    snapIt(180);
                    snapIt(270);

                    function snapIt(snapTo) {
                        if (newRotation > (snapTo - snap) && newRotation < (snapTo + snap)) {
                            newRotation = snapTo;
                        }
                    }
                    print("Start: " + startRotation  + " new: " + newRotation);
                    appletItem.rotation = newRotation;
                }
                onReleased: {
                    // save rotation
//                    print("saving...");
                    LayoutManager.saveItem(appletItem);
                }
                Rectangle { color: "red"; opacity: 0.6; visible: debug; anchors.fill: parent; }
            }
        }
        ActionButton {
            svg: configIconsSvg
            elementId: "configure"
            iconSize: root.iconSize
            visible: (action && typeof(action) != "undefined") ? action.enabled : false
            action: (applet) ? applet.action("configure") : null
            Component.onCompleted: {
                if (action && typeof(action) != "undefined") {
                    action.enabled = true
                }
            }
        }
        ActionButton {
            svg: configIconsSvg
            elementId: "size-diagonal-tr2bl" // FIXME should be maximize
            //elementId: "maximize"
            iconSize: root.iconSize
            visible: (action && typeof(action) != "undefined") ? action.enabled : false
            action: (applet) ? applet.action("run associated application") : null
            Component.onCompleted: {
                if (action && typeof(action) != "undefined") {
                    action.enabled = true
                }
            }
        }
    }

    ActionButton {
        width: handleWidth
        anchors {
            bottom: buttonsParent.bottom
            bottomMargin: (handleMerged) ? appletItem.margins.bottom + 2 : noBackgroundHandle.margins.bottom + 2
            right: appletItem.handleMerged ? parent.right : noBackgroundHandle.right
            rightMargin: appletItem.handleMerged ? -buttonMargin : noBackgroundHandle.margins.right - buttonMargin
        }

        svg: configIconsSvg
        elementId: "close"
        iconSize: root.iconSize
        visible: {
            var a = plasmoid.action("remove");
            return (a && typeof(a) != "undefined") ? a.enabled : false;
        }
        // we don't set action, since we want to catch the button click,
        // animate, and then trigger the "remove" action
        // Triggering the action is handled in the appletItem, we just
        // emit a signal here to avoid the applet-gets-removed-before-we-
        // can-animate it race condition.
        onClicked: {
            appletHandle.removeApplet();
        }
        Component.onCompleted: {
            var a = plasmoid.action("remove");
            if (a && typeof(a) != "undefined") {
                a.enabled = true
            }
        }
    }

    PlasmaCore.Svg {
        id: buttonSvg
        imagePath: "widgets/actionbutton"
    }

    Component.onCompleted: updateHeight()
}
