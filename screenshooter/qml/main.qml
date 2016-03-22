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
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import Qt.labs.controls 1.0
import Qt.labs.controls.material 1.0
import Fluid.Ui 1.0 as FluidUi

ApplicationWindow {
    property int selectedOption: 1

    id: window
    title: qsTr("Screenshot")
    width: FluidUi.Units.dp(500)
    height: FluidUi.Units.dp(500)
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height
    header: ToolBar {
        height: toolBarLayout.implicitHeight

        RowLayout {
            id: toolBarLayout
            x: spacing
            width: parent.width - (2 * spacing)
            spacing: FluidUi.Units.smallSpacing

            Button {
                text: qsTr("Cancel")
                onClicked: Qt.quit()
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: shootButton
                text: qsTr("Take Screenshot")
                onClicked: {
                    window.hide();
                    shootTimer.start();
                }
            }

            Button {
                id: actionButton
                text: qsTr("OK")
                onClicked: {
                    if (saveAction.checked)
                        fileDialog.open();
                    //else if (copyAction.checked(
                }
            }
        }
    }
    visible: true

    ColumnLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: FluidUi.Units.largeSpacing
        }
        state: "capture"
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

                    RadioButton {
                        id: wholeScreenOption
                        text: qsTr("Grab the whole screen")
                        checked: true
                        autoExclusive: true
                        onCheckedChanged: if (checked) selectedOption = 1
                    }

                    RadioButton {
                        id: activeWindowOption
                        text: qsTr("Grab the active window")
                        checked: false
                        enabled: false
                        autoExclusive: true
                        onCheckedChanged: if (checked) selectedOption = 2
                    }

                    RadioButton {
                        id: selectWindowOption
                        text: qsTr("Select a window")
                        checked: false
                        enabled: false
                        autoExclusive: true
                        onCheckedChanged: if (checked) selectedOption = 3
                    }

                    RadioButton {
                        id: selectAreaOption
                        text: qsTr("Select area to grab")
                        checked: false
                        enabled: false
                        autoExclusive: true
                        onCheckedChanged: if (checked) selectedOption = 4
                    }

                    RowLayout {
                        spacing: FluidUi.Units.smallSpacing

                        Label {
                            text: qsTr("Grab after a delay of")
                        }

                        SpinBox {
                            id: grabDelay
                            from: 0
                            to: 60
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
                    enabled: !selectAreaOption.checked
                    checked: true
                }

                CheckBox {
                    id: includeBorder
                    text: qsTr("Include the window border")
                    enabled: activeWindowOption.checked && selectWindowOption.checked
                }
            }

            Layout.fillWidth: true
        }

        ColumnLayout {
            id: previewLayout
            spacing: FluidUi.Units.smallSpacing

            GroupBox {
                title: qsTr("Preview")

                Image {
                    id: preview
                    anchors.fill: parent
                    //width: height * (sourceSize.width / sourceSize.height)
                    //height: FluidUi.Units.dp(240)
                    fillMode: Image.Stretch
                    cache: false
                }

                Layout.fillWidth: true
            }

            GroupBox {
                title: qsTr("Options")

                ColumnLayout {
                    spacing: FluidUi.Units.smallSpacing

                    RadioButton {
                        id: saveAction
                        text: qsTr("Save")
                        autoExclusive: true
                        checked: true
                    }

                    RadioButton {
                        id: copyAction
                        text: qsTr("Copy to clipboard")
                        autoExclusive: true
                        checked: false
                    }
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Timer {
        id: shootTimer
        interval: grabDelay.value * 1000
        onTriggered: {
            Screenshooter.takeScreenshot(selectedOption,
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
            window.show();
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

    Material.primary: Material.BlueGrey
    Material.accent: Material.BlueGrey
}
