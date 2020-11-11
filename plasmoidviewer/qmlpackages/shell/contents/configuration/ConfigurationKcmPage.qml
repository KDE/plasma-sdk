/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: page

    width: parent.width
    height: parent.height

//BEGIN properties
    property QtObject kcm

    signal configurationChanged
//END properties

//BEGIN functions
    function saveConfig() {
        kcm.save()
    }
//END functions

//BEGIN connections
    onKcmChanged: {
        kcm.mainUi.parent = page;
        kcm.mainUi.anchors.fill = page;
        kcm.load();
    }

    Connections {
        target: kcm
        function onNeedsSaveChanged() {
            if (kcm.needsSave) {
                page.configurationChanged();
            }
        }
    }
//END connections


}
