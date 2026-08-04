/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

import org.kde.ki18n

import org.kde.plasma.themeexplorer

QQC2.Dialog {
    id: dialog

    property alias name: nameField.text
    property alias author: authorField.text
    property alias email: emailField.text
    property alias license: licenseField.editText
    property alias website: websiteField.text
    property bool newTheme: false

    property bool canEdit: false

    standardButtons: QQC2.DialogButtonBox.Ok | QQC2.DialogButtonBox.Cancel

    Component.onCompleted: standardButton(QQC2.DialogButtonBox.Ok).enabled = Qt.binding(function(): bool {
        return dialog.canEdit && nameField.text && authorField.text && emailField.text && websiteField.text
    })


    title: newTheme ? KI18n.i18nc("@title:dialog", "New Theme") : KI18n.i18nc("@title:dialog", "Edit Theme")

    onVisibleChanged: {
        nameField.focus = true
    }

    contentItem: ColumnLayout {
        Keys.onPressed: event => {
            if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                dialog.accept();
            } else if (event.key == Qt.Key_Escape) {
                dialog.reject();
            }
        }

        QQC2.Label {
            id: errorMessage
            text: ""
            Layout.preferredHeight: visible ? implicitHeight : 0
            visible: text.length > 0
            readonly property string defaultMessage: dialog.newTheme ? "" : KI18n.i18nc("@info", "Warning: don't change author or license for themes you don't own")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: Kirigami.Units.smallSpacing

            FormLabel {
                visible: dialog.newTheme
                text: KI18n.i18nc("@label", "Theme Name:")
                buddy: nameField
            }
            QQC2.TextField {
                id: nameField
                visible: dialog.newTheme
                Layout.fillWidth: true
                onTextChanged: {
                    if (!dialog.newTheme) {
                        errorMessage.text = errorMessage.defaultMessage;
                        dialog.canEdit = true;
                        return;
                    }
                    for (var i = 0; i < themeModel.themeList.count; ++i) {
                        if (nameField.text == themeModel.themeList.get(i).packageNameRole) {
                            dialog.canEdit = false;
                            errorMessage.text = KI18n.i18nc("@info", "This theme name already exists");
                            return;
                        }
                    }
                    errorMessage.text = "";
                    dialog.canEdit = true;
                }
            }
            FormLabel {
                text: KI18n.i18nc("@label", "Author:")
                buddy: authorField
            }
            QQC2.TextField {
                id: authorField
                Layout.fillWidth: true
            }
            FormLabel {
                text: KI18n.i18nc("@label", "Email:")
                buddy: emailField
            }
            QQC2.TextField {
                id: emailField
                Layout.fillWidth: true
            }
            FormLabel {
                text: KI18n.i18nc("@label", "License:")
                buddy: licenseField
            }
            QQC2.ComboBox {
                id: licenseField
                Layout.fillWidth: true
                editable: true
                editText: "LGPL 2.1+"
                model: ["LGPL 2.1+", "GPL 2+", "GPL 3+", "LGPL 3+", "BSD"]
            }
            FormLabel {
                text: KI18n.i18nc("@label", "Website:")
                buddy: websiteField
            }
            QQC2.TextField {
                id: websiteField
                Layout.fillWidth: true
            }
        }
    }

    onAccepted: {
        if (dialog.newTheme) {
            ThemeModel.createNewTheme(nameField.text, authorField.text, emailField.text, licenseField.editText, websiteField.text);
            for (let i = 0; i < ThemeModel.themeList.count; ++i) {
                if (nameField.text == ThemeModel.themeList.get(i).packageNameRole) {
                    themeSelector.currentIndex = i;
                    break;
                }
            }
        } else {
            ThemeModel.editThemeMetaData(nameField.text, authorField.text, emailField.text, licenseField.editText, websiteField.text);
        }
    }
}
