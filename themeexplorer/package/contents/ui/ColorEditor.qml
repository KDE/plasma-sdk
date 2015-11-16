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
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import "fakecontrols" as FakeControls


Dialog {
    id: dialog
    property alias textColor: textButton.color
    property alias backgroundColor: backgroundButton.color
    property alias highlightColor: highlightButton.color
    property alias linkColor: linkButton.color
    property alias visitedLinkColor: visitedLinkButton.color

    property alias buttonTextColor: buttonTextButton.color
    property alias buttonBackgroundColor: buttonBackgroundButton.color
    property alias buttonHoverColor: buttonHoverButton.color
    property alias buttonFocusColor: buttonFocusButton.color

    property alias viewTextColor: viewTextButton.color
    property alias viewBackgroundColor: viewBackgroundButton.color
    property alias viewHoverColor: viewHoverButton.color
    property alias viewFocusColor: viewFocusButton.color

    property alias complementaryTextColor: complementaryTextButton.color
    property alias complementaryBackgroundColor: complementaryBackgroundButton.color
    property alias complementaryHoverColor: complementaryHoverButton.color
    property alias complementaryFocusColor: complementaryFocusButton.color

    title: i18n("Edit Colors");

    onVisibleChanged: {
        if (visible) {
            textColor = themeModel.colorEditor.textColor;
            backgroundColor = themeModel.colorEditor.backgroundColor;
            highlightColor = themeModel.colorEditor.highlightColor;
            linkColor = themeModel.colorEditor.linkColor;
            visitedLinkColor = themeModel.colorEditor.visitedLinkColor;

            buttonTextColor = themeModel.colorEditor.buttonTextColor;
            buttonBackgroundColor = themeModel.colorEditor.buttonBackgroundColor;
            buttonHoverColor = themeModel.colorEditor.buttonHoverColor;
            buttonFocusColor = themeModel.colorEditor.buttonFocusColor;

            viewTextColor = themeModel.colorEditor.viewTextColor;
            viewBackgroundColor = themeModel.colorEditor.viewBackgroundColor;
            viewHoverColor = themeModel.colorEditor.viewHoverColor;
            viewFocusColor = themeModel.colorEditor.viewFocusColor;

            complementaryTextColor = themeModel.colorEditor.complementaryTextColor;
            complementaryBackgroundColor = themeModel.colorEditor.complementaryBackgroundColor;
            complementaryHoverColor = themeModel.colorEditor.complementaryHoverColor;
            complementaryFocusColor = themeModel.colorEditor.complementaryFocusColor;
        }
    }
    ColorDialog {
        id: colorDialog
        modality: Qt.WindowModal
        showAlphaChannel: false
        title: i18n("Select Color")
        property Item activeButton
        onAccepted: {
            activeButton.color = color;
        }
    }
    contentItem: Rectangle {
        implicitWidth:  units.gridUnit * 50
        implicitHeight: units.gridUnit * 42

        SystemPalette {
            id: palette
        }
        color: palette.window
        
        ColumnLayout {
            anchors.fill: parent
            Rectangle {
                Layout.fillWidth: true
                //Layout.fillHeight: true
                Layout.minimumHeight: units.gridUnit * 15
                color: palette.base
                Rectangle {
                    id: plasmoidPreview
                    anchors.centerIn: parent
                    width: parent.width/1.5
                    height: parent.height/1.5
                    radius: units.smallSpacing
                    color: backgroundColor
                    ColumnLayout {
                        anchors.centerIn: parent
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: i18n("Normal text")
                            color: textColor
                        }
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                text: i18n("Link")
                                color: linkColor
                            }
                            Label {
                                text: i18n("Visited Link")
                                color: visitedLinkColor
                            }
                        }
                        FakeControls.Button {
                            Layout.alignment: Qt.AlignHCenter
                        }
                        FakeControls.LineEdit {
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
                DropShadow {
                    anchors.fill: plasmoidPreview
                    horizontalOffset: 0
                    verticalOffset: units.smallSpacing/2
                    radius: units.gridUnit / 2.2
                    samples: 16
                    color: Qt.rgba(0, 0, 0, 0.5)
                    source: plasmoidPreview
                }
            }
            ScrollView {
                id: scroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: units.gridUnit * 15
                Item {
                    width: scroll.viewport.width
                    height: childrenRect.height
                    GridLayout {
                        //Layout.alignment: Qt.AlignHCenter
                        anchors.centerIn: parent
                        columns: 2
                        columnSpacing: units.smallSpacing

                        
                        FormLabel {
                            text: i18n("Text color:")
                            buddy: textButton
                        }
                        ColorButton {
                            id: textButton
                        }
                        FormLabel {
                            text: i18n("Background color:")
                            buddy: backgroundButton
                        }
                        ColorButton {
                            id: backgroundButton
                        }
                        FormLabel {
                            text: i18n("Highlight color:")
                            buddy: highlightButton
                        }
                        ColorButton {
                            id: highlightButton
                        }
                        FormLabel {
                            text: i18n("Link color:")
                            buddy: linkButton
                        }
                        ColorButton {
                            id: linkButton
                        }
                        FormLabel {
                            text: i18n("Visited link color:")
                            buddy: visitedLinkButton
                        }
                        ColorButton {
                            id: visitedLinkButton
                        }

                        FormLabel {
                            text: i18n("Button text color:")
                            buddy: buttonTextButton
                        }
                        ColorButton {
                            id: buttonTextButton
                        }
                        FormLabel {
                            text: i18n("Button background color:")
                            buddy: buttonBackgroundButton
                        }
                        ColorButton {
                            id: buttonBackgroundButton
                        }
                        FormLabel {
                            text: i18n("Button mouse over color:")
                            buddy: buttonHoverButton
                        }
                        ColorButton {
                            id: buttonHoverButton
                        }
                        FormLabel {
                            text: i18n("Button focus color:")
                            buddy: buttonFocusButton
                        }
                        ColorButton {
                            id: buttonFocusButton
                        }

                        FormLabel {
                            text: i18n("Text view text color:")
                            buddy: viewTextButton
                        }
                        ColorButton {
                            id: viewTextButton
                        }
                        FormLabel {
                            text: i18n("Text view background color:")
                            buddy: viewBackgroundButton
                        }
                        ColorButton {
                            id: viewBackgroundButton
                        }
                        FormLabel {
                            text: i18n("Text view mouse over color:")
                            buddy: viewHoverButton
                        }
                        ColorButton {
                            id: viewHoverButton
                        }
                        FormLabel {
                            text: i18n("Text view focus color:")
                            buddy: viewFocusButton
                        }
                        ColorButton {
                            id: viewFocusButton
                        }

                        FormLabel {
                            text: i18n("Complementary text color:")
                            buddy: complementaryTextButton
                        }
                        ColorButton {
                            id: complementaryTextButton
                        }
                        FormLabel {
                            text: i18n("Complementary background color:")
                            buddy: complementaryBackgroundButton
                        }
                        ColorButton {
                            id: complementaryBackgroundButton
                        }
                        FormLabel {
                            text: i18n("Complementary mouse over color:")
                            buddy: complementaryHoverButton
                        }
                        ColorButton {
                            id: complementaryHoverButton
                        }
                        FormLabel {
                            text: i18n("Complementary focus color:")
                            buddy: complementaryFocusButton
                        }
                        ColorButton {
                            id: complementaryFocusButton
                        }
                    }
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                Button {
                    text: i18n("Ok")
                    onClicked: dialog.accept()
                }
                Button {
                    text: i18n("Cancel")
                    onClicked: dialog.reject()
                }
            }
        }
    }

    onAccepted: {
        themeModel.colorEditor.textColor = textColor;
        themeModel.colorEditor.backgroundColor = backgroundColor;
        themeModel.colorEditor.highlightColor = highlightColor;
        themeModel.colorEditor.linkColor = linkColor;
        themeModel.colorEditor.visitedLinkColor = visitedLinkColor;

        themeModel.colorEditor.buttonTextColor = buttonTextColor;
        themeModel.colorEditor.buttonBackgroundColor = buttonBackgroundColor;
        themeModel.colorEditor.buttonHoverColor = buttonHoverColor;
        themeModel.colorEditor.buttonFocusColor = buttonFocusColor;

        themeModel.colorEditor.viewTextColor = viewTextColor;
        themeModel.colorEditor.viewBackgroundColor = viewBackgroundColor;
        themeModel.colorEditor.viewHoverColor = viewHoverColor;
        themeModel.colorEditor.viewFocusColor = viewFocusColor;

        themeModel.colorEditor.complementaryTextColor = complementaryTextColor;
        themeModel.colorEditor.complementaryBackgroundColor = complementaryBackgroundColor;
        themeModel.colorEditor.complementaryHoverColor = complementaryHoverColor;
        themeModel.colorEditor.complementaryFocusColor = complementaryFocusColor;

        themeModel.colorEditor.save();
    }
}
