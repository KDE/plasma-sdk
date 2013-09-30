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
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.qtextracomponents 2.0 as QtExtras

import "plasmapackage:/code/LayoutManager.js" as LayoutManager

Item {
    id: appletItem

    anchors.rightMargin: -handleWidth*controlsOpacity

    property int handleWidth: iconSize + 8 // 4 pixels margins inside handle
    property int minimumHandleHeight: 6 * (root.iconSize + 6) + margins.top + margins.bottom
    property int handleHeight: (height < minimumHandleHeight) ? minimumHandleHeight : height
    property string category

    property bool showAppletHandle: false
    property real controlsOpacity: (plasmoid.immutable || !showAppletHandle) ? 0 : 1
    property bool handleShown: true
    property string backgroundHints: "NoBackground"
    property bool hasBackground: false
    property bool handleMerged: (height < minimumHandleHeight)
    property bool animationsEnabled: false

    property int minimumWidth: Math.max(LayoutManager.cellSize.width,
                           appletContainer.minimumWidth +
                           appletItem.contents.anchors.leftMargin +
                           appletItem.contents.anchors.rightMargin)

    property int minimumHeight: Math.max(LayoutManager.cellSize.height,
                            appletContainer.minimumHeight +
                            appletItem.contents.anchors.topMargin +
                            appletItem.contents.anchors.bottomMargin)

    property alias applet: appletContainer.applet

    property Item contents: appletContainer
    property alias margins: plasmoidBackground.margins
    property alias imagePath: plasmoidBackground.imagePath

    visible: false

    //FIXME: this delay is because backgroundHints gets updated only after a while in qml applets
    Timer {
        id: appletTimer
        interval: 50
        repeat: false
        running: false
        onTriggered: {
            updateBackgroundHints();
            applet.parent = appletContainer;
            applet.anchors.fill = appletContainer;
        }
    }

    function updateBackgroundHints() {
        hasBackground = (applet.backgroundHints != "NoBackground");
        if (applet.backgroundHints == 1) {
            appletItem.imagePath = "widgets/background";
            backgroundHints = "StandardBackground";
        } else if (applet.backgroundHints == 2) {
            appletItem.imagePath = "widgets/translucentbackground"
            backgroundHints = "TranslucentBackground";
        } else if (applet.backgroundHints == 0) {
            appletItem.imagePath = ""
            backgroundHints = "NoBackground";
        } else {
            backgroundHints = "DefaultBackground";
            appletItem.imagePath = "widgets/background";
        }
        //print("Backgroundhints changed: " + appletItem.imagePath);
    }

    QtExtras.MouseEventListener {
        id: mouseListener

        anchors { left: parent.left; top: parent.top; bottom: parent.bottom; }
        width: parent.width+handleWidth;
        z: 10

        hoverEnabled: true

        onPressAndHold: {
            if (root.pressAndHoldHandle && !plasmoid.immutable) {
                //hoverTracker.interval = 400;
                hoverTracker.restart();
            }
        }

        onContainsMouseChanged: {

            animationsEnabled = true;
            //print("Mouse is " + containsMouse);
            if (!plasmoid.immutable && containsMouse) {
                if (!root.pressAndHoldHandle) {
                    hoverTracker.interval = root.handleDelay;
                    hoverTracker.restart();
                }
            } else {
                hoverTracker.stop();
                showAppletHandle = false;
            }
        }

        PlasmaCore.FrameSvgItem {
            id: plasmoidBackground
            visible: backgroundHints != "NoBackground"
            imagePath: "widgets/background"
            anchors { left: parent.left; top: parent.top; bottom: parent.bottom; }
            width: (showAppletHandle && handleMerged) ? parent.width : parent.width-handleWidth;
            z: mouseListener.z-4
            smooth: true

            Behavior on width {
                enabled: animationsEnabled
                NumberAnimation {
                    duration: !animationsEnabled ? 0 : 250
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Connections {
            target: plasmoid
            onImmutableChanged: {
                print(" TB dragMouseArea.visible: " + plasmoid.immutable)
                dragMouseArea.visible = !plasmoid.immutable;
                showAppletHandle = false;
            }
        }
        Connections {
            target: applet
            onBusyChanged: {
                if (applet.busy) {
                    busyLoader.source = "BusyOverlay.qml"
                } else if (busyLoader.item && typeof(busyLoader.item) != "undefined") {
                    busyLoader.item.disappear();
                }
            }
            onBackgroundHintsChanged: {
                print("plasmoid.backgroundHintsChanged");
                updateBackgroundHints();
            }
        }

        Item {
            id: appletContainer
            anchors {
                fill: parent
                leftMargin: plasmoidBackground.margins.left
                rightMargin: plasmoidBackground.margins.right + handleWidth
                topMargin: plasmoidBackground.margins.top
                bottomMargin: plasmoidBackground.margins.bottom
            }
            z: mouseListener.z+1

            property QtObject applet

            onAppletChanged: {
                if (applet) {
                    appletTimer.running = true;
                }
            }
            Loader {
                id: busyLoader
                anchors.centerIn: parent
                z: appletContainer.z + 1
            }
            Component.onCompleted: PlasmaExtras.AppearAnimation {
                targetItem: appletItem
            }
        }
    }

    Component.onCompleted: {
        layoutTimer.running = true
        layoutTimer.restart()
        visible = false
        // restore rotation
    }
}
