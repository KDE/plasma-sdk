/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    Rectangle {
        id: background
        anchors {
            fill: parent
            margins: units.gridUnit
        }
        radius: 3
        color: theme.backgroundColor
        opacity: 0.6
    }
    PlasmaCore.Svg {
        id: controlsSvg
        imagePath: "widgets/containment-controls"
    }
    PlasmaCore.FrameSvgItem {
        anchors {
            left: background.left
            right: background.right
            verticalCenter: background.verticalCenter
        }
        height: maxSlider.height + minSlider.height
        imagePath: "widgets/containment-controls"
        prefix: "south"
        PlasmaCore.SvgItem {
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            svg: controlsSvg
            elementId: "south-offsetslider"
        }
        PlasmaCore.SvgItem {
            id: maxSlider
            anchors {
                top: parent.top
                right: parent.right
            }
            svg: controlsSvg
            elementId: "south-maxslider"
        }
        PlasmaCore.SvgItem {
            id: minSlider
            anchors {
                bottom: parent.bottom
                right: parent.right
            }
            svg: controlsSvg
            elementId: "south-minslider"
        }
        PlasmaCore.SvgItem {
            anchors {
                top: parent.top
                left: parent.left
            }
            svg: controlsSvg
            elementId: "south-minslider"
        }
        PlasmaCore.SvgItem {
            anchors {
                bottom: parent.bottom
                left: parent.left
            }
            svg: controlsSvg
            elementId: "south-maxslider"
        }
    }

    PlasmaComponents.Label {
        anchors {
            horizontalCenter: background.horizontalCenter
            bottom: background.bottom
        }
        text: model.imagePath
        visible: width < background.width
    }
}
