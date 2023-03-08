/*
 *   SPDX-FileCopyrightText: 2012 Viranch Mehta <viranch.mehta@gmail.com>
 *   SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

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
    PlasmaCore.SvgItem {
        svg: meterSvg
        elementId: "foreground"
        anchors.centerIn: centerScrew

        width: naturalSize.width * centerScrew.svgScale
        height: naturalSize.height * centerScrew.svgScale
    }
    PlasmaComponents.Label {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: face.bottom
        }
        text: model.imagePath
        visible: width < parent.width
    }

}
