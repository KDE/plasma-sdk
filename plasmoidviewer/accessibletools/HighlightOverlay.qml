/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick

Rectangle {
    id: overlay

    width: overlayParent?.width ?? 0
    height: overlayParent?.height ?? 0
    x: localPosition.x
    y: localPosition.y
    z: 9999999

    required property Item overlayParent
    readonly property point localPosition: overlayParent?.x, overlayParent?.y, overlayParent ? parent.mapFromGlobal(overlayParent.mapToGlobal(0, 0)) : Qt.point(0, 0)

    color: "red"
    focus: false
    opacity: 0

    Accessible.ignored: true

    Behavior on width {
        NumberAnimation {
            duration: 100
        }
    }

    Behavior on height {
        NumberAnimation {
            duration: 100
        }
    }

    Behavior on x {
        XAnimator {
            duration: 100
        }
    }

    Behavior on y {
        YAnimator {
            duration: 100
        }
    }

    SequentialAnimation {
        id: animation
        running: true
        loops: -1
        NumberAnimation {
            target: overlay
            property: "opacity"
            to: 0.3
            duration: 1000
        }
        NumberAnimation {
            target: overlay
            property: "opacity"
            to: 0
            duration: 1000
        }
    }
}
