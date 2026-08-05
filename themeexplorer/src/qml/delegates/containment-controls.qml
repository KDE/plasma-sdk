/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.ksvg as KSvg
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

Item {
    id: root

    property string imagePath

    Rectangle {
        id: backgroundRect
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }
        radius: 3
        color: Kirigami.Theme.backgroundColor
        opacity: 0.6
    }
    KSvg.FrameSvgItem {
        id: svgItem
        anchors {
            left: backgroundRect.left
            right: backgroundRect.right
            verticalCenter: backgroundRect.verticalCenter
        }
        height: maxSlider.height + minSlider.height
        imagePath: "widgets/containment-controls"
        prefix: "south"
        KSvg.SvgItem {
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            imagePath: "widgets/containment-controls"
            elementId: "south-offsetslider"
        }
        KSvg.SvgItem {
            id: maxSlider
            anchors {
                top: parent.top
                right: parent.right
            }
            imagePath: "widgets/containment-controls"
            elementId: "south-maxslider"
        }
        KSvg.SvgItem {
            id: minSlider
            anchors {
                bottom: parent.bottom
                right: parent.right
            }
            imagePath: "widgets/containment-controls"
            elementId: "south-minslider"
        }
        KSvg.SvgItem {
            anchors {
                top: parent.top
                left: parent.left
            }
            imagePath: "widgets/containment-controls"
            elementId: "south-minslider"
        }
        KSvg.SvgItem {
            anchors {
                bottom: parent.bottom
                left: parent.left
            }
            imagePath: "widgets/containment-controls"
            elementId: "south-maxslider"
        }
    }
    PlasmaComponents.Label {
        anchors.top: svgItem.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        text: root.imagePath
        wrapMode: PlasmaComponents.Label.Wrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

}
