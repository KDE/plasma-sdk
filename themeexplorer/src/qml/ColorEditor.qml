/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import "fakecontrols" as FakeControls

import org.kde.ki18n

import org.kde.plasma.themeexplorer

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

    title: KI18n.i18n("Edit Colors");

    onVisibleChanged: {
        if (visible) {
            textColor = ThemeModel.colorEditor.textColor;
            backgroundColor = ThemeModel.colorEditor.backgroundColor;
            highlightColor = ThemeModel.colorEditor.highlightColor;
            linkColor = ThemeModel.colorEditor.linkColor;
            visitedLinkColor = ThemeModel.colorEditor.visitedLinkColor;

            buttonTextColor = ThemeModel.colorEditor.buttonTextColor;
            buttonBackgroundColor = ThemeModel.colorEditor.buttonBackgroundColor;
            buttonHoverColor = ThemeModel.colorEditor.buttonHoverColor;
            buttonFocusColor = ThemeModel.colorEditor.buttonFocusColor;

            viewTextColor = ThemeModel.colorEditor.viewTextColor;
            viewBackgroundColor = ThemeModel.colorEditor.viewBackgroundColor;
            viewHoverColor = ThemeModel.colorEditor.viewHoverColor;
            viewFocusColor = ThemeModel.colorEditor.viewFocusColor;

            complementaryTextColor = ThemeModel.colorEditor.complementaryTextColor;
            complementaryBackgroundColor = ThemeModel.colorEditor.complementaryBackgroundColor;
            complementaryHoverColor = ThemeModel.colorEditor.complementaryHoverColor;
            complementaryFocusColor = ThemeModel.colorEditor.complementaryFocusColor;
        }
    }
    ColorDialog {
        id: colorDialog
        modality: Qt.WindowModal
        title: KI18n.i18n("Select Color")
        property Item activeButton
        onAccepted: {
            activeButton.color = color;
        }
    }
    contentItem: Rectangle {
        implicitWidth:  Kirigami.Units.gridUnit * 50
        implicitHeight: Kirigami.Units.gridUnit * 42

        Keys.onPressed: event => {
            if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                dialog.accept();
            } else if (event.key == Qt.Key_Escape) {
                dialog.reject();
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                //Layout.fillHeight: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 15
                color: palette.base
                Kirigami.ShadowedRectangle {
                    id: plasmoidPreview
                    anchors.centerIn: parent
                    width: parent.width/1.5
                    height: parent.height/1.5
                    radius: Kirigami.Units.smallSpacing
                    color: backgroundColor
                    shadow {
                        xOffset: 0
                        yOffset: Kirigami.Units.smallSpacing/2
                        size: Kirigami.Units.gridUnit / 2.2
                        color: Qt.rgba(0, 0, 0, 0.5)
                    }
                    ColumnLayout {
                        anchors.centerIn: parent
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: KI18n.i18n("Normal text")
                            color: textColor
                        }
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            Label {
                                text: KI18n.i18n("Link")
                                color: linkColor
                            }
                            Label {
                                text: KI18n.i18n("Visited Link")
                                color: visitedLinkColor
                            }
                        }
                        FakeControls.CheckBox {
                            Layout.alignment: Qt.AlignHCenter
                        }
                        FakeControls.LineEdit {
                            Layout.alignment: Qt.AlignHCenter
                        }
                        FakeControls.Button {
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
            Rectangle {
                id: complementaryBar
                color: complementaryBackgroundColor
                height: Kirigami.Units.gridUnit * 2
                Layout.fillWidth: true

                Label {
                    text: i18n("Complementary colors area:")
                    color: complementaryTextColor
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: complementaryBar.left
                        leftMargin: Kirigami.Units.smallspacing
                    }
                }
                RowLayout {
                    anchors {
                        top: complementaryBar.top
                        bottom: complementaryBar.bottom
                        horizontalCenter: complementaryBar.horizontalCenter
                    }
                    Label {
                        text: KI18n.i18n("Label")
                        color: complementaryTextColor
                    }
                    FakeControls.CheckBox {
                        complementary: true
                    }
                }
            }
            ScrollView {
                id: scroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 15
                Item {
                    width: parent.width
                    implicitHeight: childrenRect.height
                    GridLayout {
                        //Layout.alignment: Qt.AlignHCenter
                        anchors.centerIn: parent
                        columns: 2
                        columnSpacing: Kirigami.Units.smallSpacing

                        FormLabel {
                            text: KI18n.i18n("Text color:")
                            buddy: textButton
                        }
                        ColorButton {
                            id: textButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Background color:")
                            buddy: backgroundButton
                        }
                        ColorButton {
                            id: backgroundButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Highlight color:")
                            buddy: highlightButton
                        }
                        ColorButton {
                            id: highlightButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Link color:")
                            buddy: linkButton
                        }
                        ColorButton {
                            id: linkButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Visited link color:")
                            buddy: visitedLinkButton
                        }
                        ColorButton {
                            id: visitedLinkButton
                        }

                        FormLabel {
                            text: KI18n.i18n("Button text color:")
                            buddy: buttonTextButton
                        }
                        ColorButton {
                            id: buttonTextButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Button background color:")
                            buddy: buttonBackgroundButton
                        }
                        ColorButton {
                            id: buttonBackgroundButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Button mouse over color:")
                            buddy: buttonHoverButton
                        }
                        ColorButton {
                            id: buttonHoverButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Button focus color:")
                            buddy: buttonFocusButton
                        }
                        ColorButton {
                            id: buttonFocusButton
                        }

                        FormLabel {
                            text: KI18n.i18n("Text view text color:")
                            buddy: viewTextButton
                        }
                        ColorButton {
                            id: viewTextButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Text view background color:")
                            buddy: viewBackgroundButton
                        }
                        ColorButton {
                            id: viewBackgroundButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Text view mouse over color:")
                            buddy: viewHoverButton
                        }
                        ColorButton {
                            id: viewHoverButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Text view focus color:")
                            buddy: viewFocusButton
                        }
                        ColorButton {
                            id: viewFocusButton
                        }

                        FormLabel {
                            text: KI18n.i18n("Complementary text color:")
                            buddy: complementaryTextButton
                        }
                        ColorButton {
                            id: complementaryTextButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Complementary background color:")
                            buddy: complementaryBackgroundButton
                        }
                        ColorButton {
                            id: complementaryBackgroundButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Complementary mouse over color:")
                            buddy: complementaryHoverButton
                        }
                        ColorButton {
                            id: complementaryHoverButton
                        }
                        FormLabel {
                            text: KI18n.i18n("Complementary focus color:")
                            buddy: complementaryFocusButton
                        }
                        ColorButton {
                            id: complementaryFocusButton
                        }
                    }
                }
            }
            DialogButtonBox {
                Layout.fillWidth: true
                Button {
                    text: KI18n.i18n("OK")
                    onClicked: dialog.accept()
                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                }
                Button {
                    text: KI18n.i18n("Cancel")
                    onClicked: dialog.reject()
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                }
            }
        }
    }

    onAccepted: {
        ThemeModel.colorEditor.textColor = textColor;
        ThemeModel.colorEditor.backgroundColor = backgroundColor;
        ThemeModel.colorEditor.highlightColor = highlightColor;
        ThemeModel.colorEditor.linkColor = linkColor;
        ThemeModel.colorEditor.visitedLinkColor = visitedLinkColor;

        ThemeModel.colorEditor.buttonTextColor = buttonTextColor;
        ThemeModel.colorEditor.buttonBackgroundColor = buttonBackgroundColor;
        ThemeModel.colorEditor.buttonHoverColor = buttonHoverColor;
        ThemeModel.colorEditor.buttonFocusColor = buttonFocusColor;

        ThemeModel.colorEditor.viewTextColor = viewTextColor;
        ThemeModel.colorEditor.viewBackgroundColor = viewBackgroundColor;
        ThemeModel.colorEditor.viewHoverColor = viewHoverColor;
        ThemeModel.colorEditor.viewFocusColor = viewFocusColor;

        ThemeModel.colorEditor.complementaryTextColor = complementaryTextColor;
        ThemeModel.colorEditor.complementaryBackgroundColor = complementaryBackgroundColor;
        ThemeModel.colorEditor.complementaryHoverColor = complementaryHoverColor;
        ThemeModel.colorEditor.complementaryFocusColor = complementaryFocusColor;

        ThemeModel.colorEditor.save();
    }
}
