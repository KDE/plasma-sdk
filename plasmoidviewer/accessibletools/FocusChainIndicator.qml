/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick

Item {
    id: indicator
    anchors.fill: parent
    z: 9999999

    required property bool drawPreviousLine
    required property bool drawNextLine
    required property bool drawKeyNavigation

    property Item _activeFocusItem
    property Item _previousItemInFocusChain
    property Item _nextItemInFocusChain
    property Item _keyNavigationUp
    property Item _keyNavigationDown
    property Item _keyNavigationLeft
    property Item _keyNavigationRight
    property Item _keyNavigationTab
    property Item _keyNavigationBacktab

    readonly property point activeFocusItemCenterPosition: _activeFocusItem?.x, _activeFocusItem?.y, _activeFocusItem ? mapFromGlobal(_activeFocusItem.mapToGlobal(_activeFocusItem.width / 2, _activeFocusItem.height / 2)) : Qt.point(0, 0)
    readonly property point previousItemInFocusChainCenterPosition: _previousItemInFocusChain?.x, _previousItemInFocusChain?.y, _previousItemInFocusChain ? mapFromGlobal(_previousItemInFocusChain.mapToGlobal(_previousItemInFocusChain.width / 2, _previousItemInFocusChain.height / 2)) : Qt.point(0, 0)
    readonly property point nextItemInFocusChainCenterPosition: _nextItemInFocusChain?.x, _nextItemInFocusChain?.y, _nextItemInFocusChain ? mapFromGlobal(_nextItemInFocusChain?.mapToGlobal(_nextItemInFocusChain.width / 2, _nextItemInFocusChain.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationUpCenterPosition: _keyNavigationUp?.x, _keyNavigationUp?.y, _keyNavigationUp ? mapFromGlobal(_keyNavigationUp.mapToGlobal(_keyNavigationUp.width / 2, _keyNavigationUp.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationDownCenterPosition: _keyNavigationDown?.x, _keyNavigationDown?.y, _keyNavigationDown ? mapFromGlobal(_keyNavigationDown.mapToGlobal(_keyNavigationDown.width / 2, _keyNavigationDown.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationLeftCenterPosition: _keyNavigationLeft?.x, _keyNavigationLeft?.y, _keyNavigationLeft ? mapFromGlobal(_keyNavigationLeft.mapToGlobal(_keyNavigationLeft.width / 2, _keyNavigationLeft.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationRightCenterPosition: _keyNavigationRight?.x, _keyNavigationRight?.y, _keyNavigationRight ? mapFromGlobal(_keyNavigationRight.mapToGlobal(_keyNavigationRight.width / 2, _keyNavigationRight.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationTabCenterPosition: _keyNavigationTab?.x, _keyNavigationTab?.y, _keyNavigationTab ? mapFromGlobal(_keyNavigationTab.mapToGlobal(_keyNavigationTab.width / 2, _keyNavigationTab.height / 2)) : Qt.point(0, 0)
    readonly property point keyNavigationBacktabCenterPosition: _keyNavigationBacktab?.x, _keyNavigationBacktab?.y, _keyNavigationBacktab ? mapFromGlobal(_keyNavigationBacktab.mapToGlobal(_keyNavigationBacktab.width / 2, _keyNavigationBacktab.height / 2)) : Qt.point(0, 0)

    onActiveFocusItemCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onPreviousItemInFocusChainCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onNextItemInFocusChainCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationUpCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationDownCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationLeftCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationRightCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationTabCenterPositionChanged: Qt.callLater(canvas.requestPaint)
    onKeyNavigationBacktabCenterPositionChanged: Qt.callLater(canvas.requestPaint)

    Canvas {
        id: canvas
        anchors.fill: parent
        opacity: 0.3

        function drawLine(ctx, p0: point, p1: point) {
            ctx.beginPath()
            ctx.moveTo(p0.x, p0.y)
            ctx.lineTo(p1.x, p1.y)
            ctx.fill()
            ctx.stroke()
        }

        function drawArrow(ctx, p0: point, angleToHorizon: real, arrowAngle: real, arrowLength: real) {
            ctx.beginPath()
            ctx.moveTo(p0.x, p0.y)
            const angle1 = angleToHorizon + Math.PI + arrowAngle;
            ctx.lineTo(p0.x + Math.cos(angle1) * arrowLength, p0.y + Math.sin(angle1) * arrowLength)
            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.moveTo(p0.x, p0.y)
            const angle2 = angleToHorizon + Math.PI - arrowAngle;
            ctx.lineTo(p0.x + Math.cos(angle2) * arrowLength, p0.y + Math.sin(angle2) * arrowLength)
            ctx.fill()
            ctx.stroke()
        }

        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            if (_activeFocusItem === null) {
                return;
            }

            ctx.lineWidth = 4
            const arrowLength = 30;

            if (drawKeyNavigation) {
                ctx.strokeStyle = "green"
                ctx.fillStyle = "green"
                if (_keyNavigationUp) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationUpCenterPosition)
                }
                if (_keyNavigationDown) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationDownCenterPosition)
                }
                if (_keyNavigationLeft) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationLeftCenterPosition)
                }
                if (_keyNavigationRight) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationRightCenterPosition)
                }
                if (_keyNavigationTab && _keyNavigationTab !== _nextItemInFocusChain) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationTabCenterPosition)
                }
                if (_keyNavigationBacktab && _keyNavigationBacktab !== _previousItemInFocusChain) {
                    drawLine(ctx, activeFocusItemCenterPosition, keyNavigationBacktabCenterPosition)
                }
            }

            if (drawPreviousLine && _previousItemInFocusChain !== null && _previousItemInFocusChain !== _nextItemInFocusChain) {
                ctx.strokeStyle = "red"
                ctx.fillStyle = "red"
                drawLine(ctx, activeFocusItemCenterPosition, previousItemInFocusChainCenterPosition)
                const angleToHorizon = Math.atan2(activeFocusItemCenterPosition.y - previousItemInFocusChainCenterPosition.y, activeFocusItemCenterPosition.x - previousItemInFocusChainCenterPosition.x);
                drawArrow(ctx, activeFocusItemCenterPosition, angleToHorizon, Math.PI / 12, arrowLength)
            }

            if (drawNextLine && _nextItemInFocusChain !== null) {
                ctx.strokeStyle = "blue"
                ctx.fillStyle = "blue"
                drawLine(ctx, activeFocusItemCenterPosition, nextItemInFocusChainCenterPosition)
                const angleToHorizon = Math.atan2(nextItemInFocusChainCenterPosition.y - activeFocusItemCenterPosition.y, nextItemInFocusChainCenterPosition.x - activeFocusItemCenterPosition.x);
                drawArrow(ctx, nextItemInFocusChainCenterPosition, angleToHorizon, Math.PI / 12, arrowLength)
            }
        }
    }

    Text {
        x: nextItemInFocusChainCenterPosition.x - width / 2
        y: nextItemInFocusChainCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _nextItemInFocusChain !== null
        color: "blue"
        font.pixelSize: 16
        text: "Next"
    }

    Text {
        x: previousItemInFocusChainCenterPosition.x - width / 2
        y: previousItemInFocusChainCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _previousItemInFocusChain !== null && _previousItemInFocusChain !== _nextItemInFocusChain
        color: "red"
        font.pixelSize: 16
        text: "Prev"
    }

    Text {
        x: keyNavigationUpCenterPosition.x - width / 2
        y: keyNavigationUpCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationUp !== null
        color: "green"
        font.pixelSize: 12
        text: "Up"
    }

    Text {
        x: keyNavigationDownCenterPosition.x - width / 2
        y: keyNavigationDownCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationDown !== null
        color: "green"
        font.pixelSize: 12
        text: "Down"
    }

    Text {
        x: keyNavigationLeftCenterPosition.x - width / 2
        y: keyNavigationLeftCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationLeft !== null
        color: "green"
        font.pixelSize: 12
        text: "Left"
    }

    Text {
        x: keyNavigationRightCenterPosition.x - width / 2
        y: keyNavigationRightCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationRight !== null
        color: "green"
        font.pixelSize: 12
        text: "Right"
    }

    Text {
        x: keyNavigationTabCenterPosition.x - width / 2
        y: keyNavigationTabCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationTab !== null
        color: "green"
        font.pixelSize: 12
        text: "Tab"
    }

    Text {
        x: keyNavigationBacktabCenterPosition.x - width / 2
        y: keyNavigationBacktabCenterPosition.y - height / 2
        visible: _activeFocusItem !== null && _keyNavigationBacktab !== null
        color: "green"
        font.pixelSize: 12
        text: "Backtab"
    }
}
