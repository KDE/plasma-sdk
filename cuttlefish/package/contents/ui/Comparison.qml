import QtQuick 2.4
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.8 as Kirigami

Kirigami.OverlaySheet {
    property int comparisonSize
    background: Rectangle {
        id:background
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
    }

    onSheetOpenChanged: {
        if (sheetOpen) {
            comparisonSize = iconSize;
            comparisonSlider.value = [8, 16, 22, 32, 48, 64, 128].indexOf(comparisonSize);
        }
    }

    ColumnLayout {
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
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
                    onTriggered: comparisonSize = indexToSize(comparisonSlider.value)
                }
            }
            QQC2.Label {
                text: indexToSize(comparisonSlider.value)
            }
        }
        GridView {
            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight
            id: comparisonGrid
            cellWidth: 128
            cellHeight: 128 + 2 * Kirigami.Units.gridUnit  +  Kirigami.Units.largeSpacing
            model: sheetOpen ? iconModel.inOtherThemes(preview.iconName, comparisonSize) : []
            delegate: ColumnLayout {
                id: iconColumn
                width: comparisonGrid.cellWidth
                height: comparisonGrid.cellHeight
                spacing: Kirigami.Units.largeSpacing
                Item {
                    id: wrapper
                    Layout.preferredWidth: 128
                    Layout.preferredHeight: 128
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
}
