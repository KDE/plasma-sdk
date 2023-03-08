/*
    SPDX-FileCopyrightText: 2019 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.4
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.8 as Kirigami

Kirigami.OverlaySheet {
    property int comparisonSize
    onVisibleChanged: {
        if (visible) {
            comparisonSize = iconSize;
            comparisonSlider.value = cuttlefish.iconSizes.indexOf(comparisonSize);
        }
    }

    header: RowLayout {
        Item {
            Layout.fillWidth:true
        }
        QQC2.Slider {
            Layout.preferredWidth: comparisonGrid.width * 0.75
            id: comparisonSlider
            to: 6.0
            stepSize: 1.0
            snapMode: QQC2.Slider.SnapAlways
            onValueChanged: {
                comparisonTimer.restart()
            }

            Timer {
                id: comparisonTimer
                running: false
                repeat: false
                interval: 200
                onTriggered: comparisonSize = cuttlefish.iconSizes[comparisonSlider.value]
            }
        }
        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            text: cuttlefish.iconSizes[comparisonSlider.value]
        }
        Item {
            Layout.fillWidth:true
        }
    }
    contentItem: GridView {
        id: comparisonGrid
        cellWidth: Kirigami.Units.iconSizes.enormous
        cellHeight: cellWidth  + 2 * Kirigami.Units.gridUnit  +  Kirigami.Units.largeSpacing
        implicitWidth: {
            const availableWidth = cuttlefish.width - leftPadding - rightPadding;
            return availableWidth < comparisonGrid.count * cellWidth  ? availableWidth - availableWidth % cellWidth :  comparisonGrid.count * cellWidth
        }
        model: visible ? iconModel.inOtherThemes(preview.iconName, comparisonSize) : []
        delegate: ColumnLayout {
            id: iconColumn
            width: comparisonGrid.cellWidth
            height: comparisonGrid.cellHeight
            spacing: Kirigami.Units.largeSpacing
            Item {
                id: wrapper
                Layout.preferredWidth: comparisonGrid.cellWidth
                Layout.preferredHeight: comparisonGrid.cellWidth
                Kirigami.Icon {
                    property bool hasIcon : modelData.iconPath
                    anchors.centerIn: parent
                    width: comparisonSize
                    height: width
                    source: hasIcon ? modelData.iconPath : "paint-none"
                    color: hasIcon ? "" : Kirigami.Theme.disabledTextColor
                    isMask: !hasIcon
                }
            }
            QQC2.Label {
                id: themeName
                text: modelData.themeName
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            }
        }
    }
}
