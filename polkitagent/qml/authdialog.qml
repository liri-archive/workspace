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
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.UI 1.0

ApplicationWindow {
    id: authenticationDialog

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

    title: qsTr("Authentication required")
    color: "transparent"
    onAvatarChanged: {
        // Load the image from the disk if it's an absolute path
        if (avatar.indexOf("/") == 0) {
            avatarImage.name = "";
            avatarImage.source = avatar;
        }

        // Otherwise use a standard icon
        avatarImage.source = "";
        avatarImage.name = "action/verified_user";
    }

    Popup {
        id: popup
        x: (parent.width - popup.width) / 2
        y: (parent.height - popup.height) / 2
        width: 300
        height: 300
        modal: true
        focus: true

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            Keys.onEscapePressed: cancelButton.clicked()

            RowLayout {
                spacing: 8

                Icon {
                    name: "action/lock"
                    size: 48

                    Layout.alignment: Qt.AlignTop
                }

                ColumnLayout {
                    spacing: 8

                    TitleLabel {
                        text: qsTr("Authentication required")
                    }

                    Label {
                        id: messageLabel
                        wrapMode: Text.WordWrap

                        Layout.fillWidth: true
                    }

                    RowLayout {
                        spacing: 8

                        Icon {
                            id: avatarImage
                            size: 64
                        }

                        Label {
                            id: avatarName

                            Layout.alignment: Qt.AlignVCenter
                        }

                        Layout.alignment: Qt.AlignCenter
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        spacing: 8

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
                spacing: 8

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

    function open() {
        visibility = Window.FullScreen;
        popup.open();
    }

    function close() {
        visibility = Window.Hidden;
        popup.close();
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
}
