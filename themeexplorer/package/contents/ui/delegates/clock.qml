/*
 *   Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
 *   Copyright 2012 Marco Martin <mart@kde.org>
 *   Copyright 2013 David Edmundson <davidedmundson@kde.org>
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
import org.kde.plasma.calendar 2.0 as PlasmaCalendar
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: analogclock

    width: units.gridUnit * 15
    height: units.gridUnit * 15
    property int hours
    property int minutes
    property int seconds
    property bool showSecondsHand: true

 
    PlasmaCore.DataSource {
        id: dataSource
        engine: "time"
        connectedSources: "Local"
        interval: showSecondsHand ? 1000 : 30000
        onDataChanged: {
            var date = new Date(data["Local"]["DateTime"]);
            hours = date.getHours();
            minutes = date.getMinutes();
            seconds = date.getSeconds();
        }
        Component.onCompleted: {
            onDataChanged();
        }
    }


    Component.onCompleted: {
        dataSource.onDataChanged.connect(dateTimeChanged);
    }


    PlasmaCore.Svg {
        id: clockSvg
        imagePath: "widgets/clock"
        function estimateHorizontalHandShadowOffset() {
            var id = "hint-hands-shadow-offset-to-west";
            if (hasElement(id)) {
                return -elementSize(id).width;
            }
            id = "hint-hands-shadows-offset-to-east";
            if (hasElement(id)) {
                return elementSize(id).width;
            }
            return 0;
        }
        function estimateVerticalHandShadowOffset() {
            var id = "hint-hands-shadow-offset-to-north";
            if (hasElement(id)) {
                return -elementSize(id).height;
            }
            id = "hint-hands-shadow-offset-to-south";
            if (hasElement(id)) {
                return elementSize(id).height;
            }
            return 0;
        }
        property double naturalHorizontalHandShadowOffset: estimateHorizontalHandShadowOffset()
        property double naturalVerticalHandShadowOffset: estimateVerticalHandShadowOffset()
        onRepaintNeeded: {
            naturalHorizontalHandShadowOffset = estimateHorizontalHandShadowOffset();
            naturalVerticalHandShadowOffset = estimateVerticalHandShadowOffset();
        }
    }

    Item {
        id: clock
        width: parent.width
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        readonly property double svgScale: face.width / face.naturalSize.width
        readonly property double horizontalShadowOffset:
            Math.round(clockSvg.naturalHorizontalHandShadowOffset * svgScale) + Math.round(clockSvg.naturalHorizontalHandShadowOffset * svgScale) % 2
        readonly property double verticalShadowOffset:
            Math.round(clockSvg.naturalVerticalHandShadowOffset * svgScale) + Math.round(clockSvg.naturalVerticalHandShadowOffset * svgScale) % 2

        PlasmaCore.SvgItem {
            id: face
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: Math.min(parent.width, parent.height)
            svg: clockSvg
            elementId: "ClockFace"
        }

        Hand {
            elementId: "HourHandShadow"
            rotationCenterHintId: "hint-hourhandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + hours * 30 + (minutes/2)
            svgScale: clock.svgScale

        }
        Hand {
            elementId: "HourHand"
            rotationCenterHintId: "hint-hourhand-rotation-center-offset"
            rotation: 180 + hours * 30 + (minutes/2)
            svgScale: clock.svgScale
        }

        Hand {
            elementId: "MinuteHandShadow"
            rotationCenterHintId: "hint-minutehandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + minutes * 6
            svgScale: clock.svgScale
        }
        Hand {
            elementId: "MinuteHand"
            rotationCenterHintId: "hint-minutehand-rotation-center-offset"
            rotation: 180 + minutes * 6
            svgScale: clock.svgScale
        }

        Hand {
            elementId: "SecondHandShadow"
            rotationCenterHintId: "hint-secondhandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + seconds * 6
            visible: showSecondsHand
            svgScale: clock.svgScale
        }
        Hand {
            elementId: "SecondHand"
            rotationCenterHintId: "hint-secondhand-rotation-center-offset"
            rotation: 180 + seconds * 6
            visible: showSecondsHand
            svgScale: clock.svgScale
        }

        PlasmaCore.SvgItem {
            id: center
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
            anchors.centerIn: clock
            svg: clockSvg
            elementId: "HandCenterScrew"
            z: 1000
        }

        PlasmaCore.SvgItem {
            anchors.fill: face
            svg: clockSvg
            elementId: "Glass"
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
        }
    }
}
