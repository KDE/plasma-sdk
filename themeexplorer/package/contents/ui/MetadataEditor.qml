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
    property alias name: nameField.text
    property alias author: authorField.text
    property alias email: emailField.text
    property alias website: websiteField.text
    property bool newTheme: false

    property bool canEdit: false

    title: newTheme ? i18n("New Theme") : i18n("Edit Theme")
    standardButtons: canEdit && nameField.text && authorField.text && emailField.text && websiteField.text ? StandardButton.Ok | StandardButton.Cancel : StandardButton.Cancel

    onVisibleChanged: {
        nameField.focus = true
    }
    ColumnLayout {
        Label {
            id: errorMessage
        }
        Grid {
            columns: 2
            spacing: units.smallSpacing
            //anchors.fill:parent
            horizontalItemAlignment: Grid.AlignRight 
            verticalItemAlignment: Grid.AlignVCenter
            Label {
                visible: newTheme
                text: i18n("Theme Name:")
                MouseArea {
                    anchors.fill: parent
                    onClicked: nameField.focus = true
                }
            }
            TextField {
                id: nameField
                visible: newTheme
                onTextChanged: {
                    if (!newTheme) {
                        errorMessage.text = "";
                        dialog.canEdit = true;
                        return;
                    }
                    for (var i = 0; i < themeModel.themeList.count; ++i) {
                        if (nameField.text == themeModel.themeList.get(i).packageNameRole) {
                            dialog.canEdit = false;
                            errorMessage.text = i18n("This theme name already exists");
                            return;
                        }
                    }
                    errorMessage.text = "";
                    dialog.canEdit = true;
                }
            }
            Label {
                text: i18n("Author:")
                MouseArea {
                    anchors.fill: parent
                    onClicked: authorField.focus = true
                }
            }
            TextField {
                id: authorField
            }
            Label {
                text: i18n("Email:")
                MouseArea {
                    anchors.fill: parent
                    onClicked: emailField.focus = true
                }
            }
            TextField {
                id: emailField
            }
            Label {
                text: i18n("Website:")
                MouseArea {
                    anchors.fill: parent
                    onClicked: websiteField.focus = true
                }
            }
            TextField {
                id: websiteField
            }
        }
    }

    onAccepted: {
        if (newTheme) {
            themeModel.createNewTheme(nameField.text, authorField.text, emailField.text, websiteField.text);
            for (var i = 0; i < themeModel.themeList.count; ++i) {
                if (nameField.text == themeModel.themeList.get(i).packageNameRole) {
                    themeSelector.currentIndex = i;
                    break;
                }
            }
        } else {
            themeModel.editThemeMetaData(nameField.text, authorField.text, emailField.text, websiteField.text);
        }
    }
}
