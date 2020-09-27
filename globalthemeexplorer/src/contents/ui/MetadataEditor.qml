/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import org.kde.kirigami 2.3 as Kirigami
import org.kde.plasma.sdk 1.0 as SDK

QQC2.Dialog {
    id: dialog
    property alias pluginName: pluginNameField.text
    property alias name: nameField.text
    property alias comment: commentField.text
    property alias author: authorField.text
    property alias email: emailField.text
    property alias license: licenseField.editText
    property alias website: websiteField.text

    property bool canEdit: false

    width: 500
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    title: i18n("New Theme")

    onVisibleChanged: {
        nameField.focus = true
    }

    contentItem: Rectangle {
        SystemPalette {
            id: palette
        }
        color: palette.window

        ColumnLayout {
            id: layout
            anchors {
                fill: parent
                margins: units.smallSpacing
            }
            Label {
                id: errorMessage
                text: ""
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            Kirigami.FormLayout {
                Layout.fillWidth: true
                QQC2.TextField {
                    id: pluginNameField
                    Layout.fillWidth: true
                    Kirigami.FormData.label: i18n("Theme Plugin Name:")
                    onTextChanged: {
                        for (var i = 0; i < SDK.GlobalTheme.themeList.count; ++i) {
                            if (pluginNameField.text == SDK.GlobalTheme.themeList.get(i).packageNameRole) {
                                dialog.canEdit = false;
                                errorMessage.text = i18n("This theme plugin name already exists");
                                return;
                            }
                        }
                        errorMessage.text = "";
                        dialog.canEdit = true;
                    }
                }
                QQC2.TextField {
                    id: "nameField"
                    Kirigami.FormData.label: i18n("Theme Name:")
                }
                QQC2.TextField {
                    id: "commentField"
                    Kirigami.FormData.label: i18n("Comment:")
                }
                QQC2.TextField {
                    id: "authorField"
                    Kirigami.FormData.label: i18n("Author:")
                }
                QQC2.TextField {
                    id: "emailField"
                    Kirigami.FormData.label:  i18n("Email:")
                }
                QQC2.TextField {
                    id: "versionField"
                    Kirigami.FormData.label: i18n("Version:")
                }
                QQC2.TextField {
                    id: "websiteField"
                    Kirigami.FormData.label:  i18n("Website:")
                }
                ComboBox {
                    id: licenseField
                    Layout.fillWidth: true
                    Kirigami.FormData.label: i18n("License:")
                    editable: true
                    editText: "LGPL 2.1+"
                    model: ["LGPL 2.1+", "GPL 2+", "GPL 3+", "LGPL 3+", "BSD"]
                }
            }
        }
    }


    footer: QQC2.DialogButtonBox {
        QQC2.Button {
            text: i18n("OK ")
            enabled: canEdit && nameField.text && authorField.text && emailField.text && websiteField.text
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
            onClicked: dialog.accept()
        }
        QQC2.Button {
            text: i18n("Cancel")
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.DestructiveRole
            onClicked: dialog.reject()
        }
    }

    onAccepted: {
        SDK.GlobalTheme.createNewTheme(pluginNameField.text, nameField.text, commentField.text, authorField.text, emailField.text, licenseField.editText, websiteField.text);
        for (var i = 0; i < SDK.GlobalTheme.themeList.count; ++i) {
            if (nameField.text == SDK.GlobalTheme.themeList.get(i).packageNameRole) {
                themeSelector.currentIndex = i;
                break;
            }
        }
    }
}
