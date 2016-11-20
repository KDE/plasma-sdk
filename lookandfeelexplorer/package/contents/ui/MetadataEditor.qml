/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

Dialog {
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
    title: i18n("New Theme")

    onVisibleChanged: {
        nameField.focus = true
    }

    //all this reimplementing shouldn't be necessary,
    //but unfortunately native standard buttons management
    //is completely broken
    contentItem: Rectangle {
        implicitWidth:  layout.Layout.minimumWidth + units.smallSpacing*2
        implicitHeight: layout.Layout.minimumHeight + units.smallSpacing*2

        Keys.onPressed: {
            if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                dialog.accept();
            } else if (event.key == Qt.Key_Escape) {
                dialog.reject();
            }
        }

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
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: units.smallSpacing

                FormLabel {
                    text: i18n("Theme Plugin Name:")
                    buddy: pluginNameField
                }
                TextField {
                    id: pluginNameField
                    Layout.fillWidth: true
                    onTextChanged: {
                        for (var i = 0; i < lnfLogic.lnfList.count; ++i) {
                            if (pluginNameField.text == lnfLogic.lnfList.get(i).packageNameRole) {
                                dialog.canEdit = false;
                                errorMessage.text = i18n("This theme plugin name already exists");
                                return;
                            }
                        }
                        errorMessage.text = "";
                        dialog.canEdit = true;
                    }
                }
                FormLabel {
                    text: i18n("Theme Name:")
                    buddy: nameField
                }
                TextField {
                    id: nameField
                    Layout.fillWidth: true
                    onTextChanged: {
                        for (var i = 0; i < lnfLogic.lnfList.count; ++i) {
                            if (nameField.text == lnfLogic.lnfList.get(i).displayRole) {
                                dialog.canEdit = false;
                                errorMessage.text = i18n("This theme name already exists");
                                return;
                            }
                        }
                        errorMessage.text = "";
                        dialog.canEdit = true;
                    }
                }
                FormLabel {
                    text: i18n("Comment:")
                    buddy: commentField
                }
                TextField {
                    id: commentField
                    Layout.fillWidth: true
                }
                FormLabel {
                    text: i18n("Author:")
                    buddy: authorField
                }
                TextField {
                    id: authorField
                    Layout.fillWidth: true
                }
                FormLabel {
                    text: i18n("Email:")
                    buddy: emailField
                }
                TextField {
                    id: emailField
                    Layout.fillWidth: true
                }
                FormLabel {
                    text: i18n("License:")
                    buddy: licenseField
                }
                ComboBox {
                    id: licenseField
                    Layout.fillWidth: true
                    editable: true
                    editText: "LGPL 2.1+"
                    model: ["LGPL 2.1+", "GPL 2+", "GPL 3+", "LGPL 3+", "BSD"]
                }
                FormLabel {
                    text: i18n("Website:")
                    buddy: websiteField
                }
                TextField {
                    id: websiteField
                    Layout.fillWidth: true
                }
            }
            Item {
                Layout.fillHeight: true
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                Button {
                    text: i18n("OK")
                    onClicked: dialog.accept()
                    isDefault: true
                    enabled: canEdit && nameField.text && authorField.text && emailField.text && websiteField.text
                }
                Button {
                    text: i18n("Cancel")
                    onClicked: dialog.reject()
                }
            }
        }
    }

    onAccepted: {
        lnfLogic.createNewTheme(pluginNameField.text, nameField.text, commentField.text, authorField.text, emailField.text, licenseField.editText, websiteField.text);
        for (var i = 0; i < lnfLogic.lnfList.count; ++i) {
            if (nameField.text == lnfLogic.lnfList.get(i).packageNameRole) {
                themeSelector.currentIndex = i;
                break;
            }
        }
    }
}
