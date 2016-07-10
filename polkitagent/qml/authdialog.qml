/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.UI 1.0 as FluidUi

ApplicationWindow {
    property string actionId
    property alias message: messageLabel.text
    property string iconName
    property alias realName: avatarName.text
    property string avatar
    property alias prompt: promptLabel.text
    property bool echo: false
    property alias infoMessage: infoLabel.text
    property alias errorMessage: errorLabel.text

    signal authenticationReady(string response)
    signal authenticationCanceled()

    id: authenticationDialog
    title: qsTr("Authentication required")
    minimumWidth: mainLayout.implicitWidth
    minimumHeight: mainLayout.implicitHeight
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    function open() {
        visible = true;
    }

    function close() {
        visible = false;
        cleanup();
    }

    function cleanup() {
        actionId = "";
        message = "";
        iconName = "";
        realName = "";
        avatar = "";
        prompt = "";
        passwordInput.text = "";
        infoMessage = "";
        errorMessage = "";
    }

    ColumnLayout {
        id: mainLayout
        spacing: FluidUi.Units.largeSpacing

        Keys.onEscapePressed: cancelButton.clicked()

        RowLayout {
            spacing: FluidUi.Units.smallSpacing

            FluidUi.Icon {
                name: "dialog-password-symbolic"
                width: FluidUi.Units.iconSizes.large
                height: width
                color: messageLabel.color

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                spacing: FluidUi.Units.smallSpacing

                Label {
                    text: qsTr("Authentication required")
                    font.bold: true
                }

                Label {
                    id: messageLabel
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: FluidUi.Units.smallSpacing

                    Image {
                        id: avatarImage
                        source: {
                            // Fallback to a default icon
                            if (avatar == "")
                                return "image://desktoptheme/avatar-default";

                            // Load the image from the disk if it's an absolute path
                            if (avatar.indexOf("/") == 0)
                                return avatar;

                            // Load from the icon theme
                            return "image://desktoptheme/" + avatar;
                        }
                        sourceSize.width: width
                        sourceSize.height: height
                        width: FluidUi.Units.iconSizes.large
                        height: width
                        smooth: true
                        cache: false
                    }

                    Label {
                        id: avatarName

                        Layout.alignment: Qt.AlignVCenter
                    }

                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: FluidUi.Units.smallSpacing

                    Label {
                        id: promptLabel
                    }

                    TextField {
                        id: passwordInput
                        echoMode: echo ? TextInput.Normal : TextInput.Password
                        focus: true
                        onAccepted: okButton.clicked()

                        Layout.fillWidth: true
                    }
                }

                Label {
                    id: infoLabel
                    color: "green"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    visible: text != ""

                    Layout.fillWidth: true
                }

                Label {
                    id: errorLabel
                    color: "red"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    visible: text != ""

                    Layout.fillWidth: true
                }
            }

            Layout.fillHeight: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: FluidUi.Units.smallSpacing

            Button {
                id: cancelButton
                text: qsTr("Cancel")
                onClicked: authenticationDialog.authenticationCanceled()
            }

            Button {
                id: okButton
                text: qsTr("Authenticate")
                onClicked: authenticationDialog.authenticationReady(passwordInput.text)
            }

            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
        }
    }
}
