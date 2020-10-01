/*
    SPDX-FileCopyrightText: 2019 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.2
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.8 as Kirigami

Item {
    id: dualMontage
    visible: false
    height: 512
    width: 512
    Kirigami.Theme.inherit: false
    function shot() {
        ssPicker.open()
    }
    FileDialog {
        id: ssPicker
        selectExisting: false
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            dualMontage.grabToImage(function(result) {
                res = result.saveToFile(ssPicker.fileUrl.toString().slice(7))
            });
        }
        nameFilters: [ "PNG screenshot files (*.png)" ]
    }
    Column {
        IconMontage {
            height: 256
            width: 512
            showWatermark: true

            Kirigami.Theme.inherit: false
            Kirigami.Theme.textColor: "#232629"
            Kirigami.Theme.backgroundColor: "#eff0f1"
            Kirigami.Theme.highlightColor: "#3daee9"
            Kirigami.Theme.highlightedTextColor: "#eff0f1"
            Kirigami.Theme.positiveTextColor: "#27ae60"
            Kirigami.Theme.neutralTextColor: "#f67400"
            Kirigami.Theme.negativeTextColor: "#da4453"
        }
        IconMontage {
            height: 256
            width: 512
            showWatermark: false

            Kirigami.Theme.inherit: false
            Kirigami.Theme.textColor: "#eff0f1"
            Kirigami.Theme.backgroundColor: "#31363b"
            Kirigami.Theme.highlightColor: "#3daee9"
            Kirigami.Theme.highlightedTextColor: "#eff0f1"
            Kirigami.Theme.positiveTextColor: "#27ae60"
            Kirigami.Theme.neutralTextColor: "#f67400"
            Kirigami.Theme.negativeTextColor: "#da4453"
        }
    }
}
