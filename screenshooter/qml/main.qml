/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import Fluid.Ui 1.0 as FluidUi

ApplicationWindow {
    id: root
    visible: true

    ColumnLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: FluidUi.Units.largeSpacing
        }
        states: [
            State {
                name: "capture"
                PropertyChanges { target: captureGroupBox; visible: true }
                PropertyChanges { target: effectsGroupBox; visible: true }
                PropertyChanges { target: previewLayout; visible: false }
                PropertyChanges { target: shootButton; visible: true }
                PropertyChanges { target: actionButton; visible: false }
            },
            State {
                name: "preview"
                PropertyChanges { target: captureGroupBox; visible: false }
                PropertyChanges { target: effectsGroupBox; visible: false }
                PropertyChanges { target: previewLayout; visible: true }
                PropertyChanges { target: shootButton; visible: false }
                PropertyChanges { target: actionButton; visible: true }
            }
        ]

        Keys.onEscapePressed: Qt.quit()

        GroupBox {
            id: captureGroupBox
            title: qsTr("Take Screenshot")

            ColumnLayout {
                spacing: FluidUi.Units.smallSpacing

                FluidUi.Icon {
                    iconName: "applets-screenshooter"
                }

                ColumnLayout {
                    spacing: FluidUi.Units.smallSpacing

                    ExclusiveGroup { id: options }

                    RadioButton {
                        property int what: 1

                        text: qsTr("Grab the whole screen")
                        exclusiveGroup: options
                        checked: true
                    }

                    RadioButton {
                        property int what: 2

                        text: qsTr("Grab the active window")
                        exclusiveGroup: options
                        checked: false
                        enabled: false
                    }

                    RadioButton {
                        property int what: 3

                        text: qsTr("Select a window")
                        exclusiveGroup: options
                        checked: false
                        enabled: false
                    }

                    RadioButton {
                        property int what: 4

                        text: qsTr("Select area to grab")
                        exclusiveGroup: options
                        checked: false
                        enabled: false
                    }

                    RowLayout {
                        Label {
                            text: qsTr("Grab after a delay of")
                        }

                        SpinBox {
                            id: grabDelay
                            minimumValue: 0
                            maximumValue: 60
                            value: 2
                        }

                        Label {
                            text: qsTr("seconds")
                        }
                    }
                }
            }

            Layout.fillWidth: true
        }

        GroupBox {
            id: effectsGroupBox
            title: qsTr("Effects")

            ColumnLayout {
                spacing: FluidUi.Units.smallSpacing

                CheckBox {
                    id: includePointer
                    text: qsTr("Include pointer")
                    enabled: options.current.what !== 4
                    checked: true
                }

                CheckBox {
                    id: includeBorder
                    text: qsTr("Include the window border")
                    enabled: options.current.what === 2 || options.current.what === 3
                }
            }

            Layout.fillWidth: true
        }

        ColumnLayout {
            id: previewLayout
            spacing: FluidUi.Units.smallSpacing

            GroupBox {
                title: qsTr("Preview")

                Row {
                    anchors {
                        fill: parent
                        margins: FluidUi.Units.smallSpacing
                    }

                    Image {
                        id: preview
                        width: height * (sourceSize.width / sourceSize.height)
                        height: 240
                        cache: false
                    }
                }
            }

            GroupBox {
                title: qsTr("Options")

                ColumnLayout {
                    spacing: FluidUi.Units.smallSpacing

                    ExclusiveGroup { id: actions }

                    RadioButton {
                        objectName: "saveAction"
                        text: qsTr("Save")
                        exclusiveGroup: actions
                        checked: true
                    }

                    RadioButton {
                        objectName: "copyAction"
                        text: qsTr("Copy to clipboard")
                        exclusiveGroup: actions
                        checked: false
                    }
                }

                Layout.fillWidth: true
            }
        }

        RowLayout {
            spacing: FluidUi.Units.smallSpacing

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Cancel")
                onClicked: Qt.quit()
            }

            Button {
                id: shootButton
                text: qsTr("Take Screenshot")
                onClicked: {
                    root.hide();
                    shootTimer.start();
                }
            }

            Button {
                id: actionButton
                text: qsTr("OK")
                onClicked: {
                    switch (actions.current.objectName) {
                    case "saveAction":
                        fileDialog.open();
                        break;
                    case "copyAction":
                        break;
                    }
                }
            }
        }
    }

    Timer {
        id: shootTimer
        interval: grabDelay.value * 1000
        onTriggered: {
            Screenshooter.takeScreenshot(options.current.what,
                                         includePointer.enabled && includePointer.checked,
                                         includeBorder.enabled && includeBorder.checked);
        }
    }

    Timer {
        id: showTimer
        interval: 1000
        onTriggered: {
            preview.source = "image://screenshooter/last";
            mainLayout.state = "preview";
            root.show();
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Save Screenshot As")
        folder: shortcuts.pictures
        selectExisting: false
        nameFilters: [ qsTr("Image files (*.jpg *.jpeg *.png)"), qsTr("All files (*)") ]
        onAccepted: {
            Screenshooter.saveScreenshot(fileDialog.fileUrl);
            Qt.quit();
        }
    }

    Connections {
        target: Screenshooter
        onScreenshotDone: showTimer.start()
    }

    Component.onCompleted: mainLayout.state = "capture"
}
