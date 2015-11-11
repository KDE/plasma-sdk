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
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

Dialog {
    title: i18n("New Theme")
    standardButtons: nameField.text && authorField.text && emailField.text && websiteField.text ? StandardButton.Ok | StandardButton.Cancel : StandardButton.Cancel

    Grid {
        columns: 2
        spacing: units.smallSpacing
        //anchors.fill:parent
        horizontalItemAlignment: Grid.AlignRight 
        verticalItemAlignment: Grid.AlignVCenter
        Label {
            text: i18n("Theme Name:")
        }
        TextField {
            id: nameField
        }
        Label {
            text: i18n("Author:")
        }
        TextField {
            id: authorField
        }
        Label {
            text: i18n("Email:")
        }
        TextField {
            id: emailField
        }
        Label {
            text: i18n("Website:")
        }
        TextField {
            id: websiteField
        }
    }

    onAccepted: {
        themeModel.createNewTheme(nameField.text, authorField.text, emailField.text, websiteField.text);
    }
}
