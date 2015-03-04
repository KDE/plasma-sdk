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

    property int value: 30

    PlasmaCore.Svg {
        id: meterSvg
        imagePath: "widgets/analog_meter"
    }

    PlasmaCore.SvgItem {
        id: face
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height)
        height: width / (naturalSize.width / naturalSize.height)
        svg: meterSvg
        elementId: "background"
    }

    PlasmaCore.SvgItem {
        id: centerScrew
        svg: meterSvg
        elementId: "rotatecenter"
        rotation: value + 90

        x: face.x + meterSvg.elementRect("rotatecenter").x * svgScale
        y: face.y + meterSvg.elementRect("rotatecenter").y * svgScale

        property real svgScale: face.width / face.naturalSize.width
        width: naturalSize.width * svgScale
        height: naturalSize.height * svgScale

        PlasmaCore.SvgItem {
            svg: meterSvg

            anchors.horizontalCenter: parent.horizontalCenter
            y: x
            elementId: "pointer-shadow"
            width: naturalSize.width * centerScrew.svgScale
            height: naturalSize.height * centerScrew.svgScale
        }
        PlasmaCore.SvgItem {
            id: hand
            svg: meterSvg
            elementId: "pointer"
            anchors.horizontalCenter: parent.horizontalCenter
            y: x

            width: naturalSize.width * centerScrew.svgScale
            height: naturalSize.height * centerScrew.svgScale
        }
    }

}
