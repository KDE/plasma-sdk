/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.2 as Dialogs

import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore

Kirigami.Dialog {
    id: dialog

    property alias name: nameField.text
    property alias author: authorField.text
    property alias email: emailField.text
    property alias license: licenseField.editText
    property alias website: websiteField.text
    property bool newTheme: false

    // name is non-empty and available
    property bool nameIsAcceptable: name.length !== 0 && packageNameAvailable(name)

    function packageNameAvailable(name: string): bool {
        for (let i = 0; i < themeModel.themeList.count; ++i) {
            if (name === themeModel.themeList.get(i).packageNameRole) {
                return false;
            }
        }
        return true;
    }

    title: newTheme ? i18n("New Theme") : i18n("Edit Theme")
    padding: Kirigami.Units.smallSpacing

    onOpened: {
        nameField.forceActiveFocus();
    }

    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.InlineMessage {
            text: i18n("Warning: don't change author or license for themes you don't own.")
            type: Kirigami.MessageType.Information
            visible: !dialog.newTheme
            Layout.fillWidth: true
        }

        Kirigami.InlineMessage {
            text: i18n("A theme with such name already exists.")
            type: Kirigami.MessageType.Error
            visible: dialog.newTheme && !dialog.packageNameAvailable(dialog.name)
            Layout.fillWidth: true
        }

        Kirigami.FormLayout {

            QQC2.TextField {
                id: nameField
                Kirigami.FormData.label: i18n("Theme Name:")
                Layout.fillWidth: true
                readOnly: !newTheme
            }

            QQC2.TextField {
                id: authorField
                Kirigami.FormData.label: i18n("Author:")
                Layout.fillWidth: true
            }

            QQC2.TextField {
                id: emailField
                Kirigami.FormData.label: i18n("Email:")
                Layout.fillWidth: true
            }

            QQC2.ComboBox {
                id: licenseField
                Kirigami.FormData.label: i18n("License:")
                Layout.fillWidth: true
                editable: true
                editText: "LGPL 2.1+"
                model: ["LGPL 2.1+", "GPL 2+", "GPL 3+", "LGPL 3+", "BSD"]
            }

            QQC2.TextField {
                id: websiteField
                Kirigami.FormData.label: i18n("Website:")
                Layout.fillWidth: true
            }
        }
    }

    standardButtons: QQC2.Dialog.Cancel

    // can't bind enabled property of a standard OK button, because it is well-hidden within Dialog
    customFooterActions: Kirigami.Action {
        id: actionOk
        text: i18n("OK")
        icon.name: "dialog-ok"
        shortcut: Qt.Key_Enter
        onTriggered: dialog.accept()

        enabled: (nameIsAcceptable || !newTheme) && author && email && website
    }

    onAccepted: {
        if (newTheme) {
            themeModel.createNewTheme(name, author, email, license, website);
            themeSelector.currentIndex = themeSelector.indexOfValue(name);
        } else {
            themeModel.editThemeMetaData(name, author, email, license, website);
        }
    }
}
