/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Qt.labs.platform 1.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial

FluidControls.ApplicationWindow {
    id: window

    title: qsTr("Screenshot")

    width: 500
    height: 350
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height

    visible: true

    Material.primary: Material.BlueGrey
    Material.accent: Material.BlueGrey

    initialPage: capturePage

    FluidControls.Page {
        id: capturePage

        title: qsTr("Screen capture")

        footer: DialogButtonBox {
            Button {
                text: qsTr("Capture")
                enabled: Screenshooter.enabled
                flat: true
                onClicked: {
                    window.hide();
                    shootTimer.start();
                }
            }
        }

        Column {
            anchors.fill: parent

            FluidControls.ListItem {
                text: qsTr("Grab")
                rightItem: ComboBox {
                    id: optionCombo
                    anchors.verticalCenter: parent.verticalCenter
                    textRole: "key"
                    model: ListModel {
                        ListElement { key: qsTr("This screen"); value: "screen" }
                        ListElement { key: qsTr("Active window"); value: "active-window" }
                        ListElement { key: qsTr("Specific window"); value: "select-window" }
                        ListElement { key: qsTr("Area"); value: "select-area" }
                    }
                    implicitWidth: 200
                }
            }

            FluidControls.ListItem {
                text: qsTr("Delay")
                rightItem: SpinBox {
                    id: grabDelay
                    anchors.verticalCenter: parent.verticalCenter
                    from: 0
                    to: 60
                    value: 2
                }
            }

            FluidControls.ListItem {
                text: qsTr("Include pointer")
                rightItem: Switch {
                    id: includePointer
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: optionCombo.currentIndex != 3
                    checked: true
                }
            }

            FluidControls.ListItem {
                text: qsTr("Include the window border")
                rightItem: Switch {
                    id: includeBorder
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: optionCombo.currentIndex >= 1 && optionCombo.currentIndex <= 2
                }
            }
        }
    }

    Component {
        id: previewPage

        FluidControls.Page {
            title: qsTr("Screen capture")

            footer: DialogButtonBox {
                Button {
                    text: qsTr("Save")
                    flat: true
                    onClicked: fileDialog.open()

                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                }

                Button {
                    text: qsTr("Copy to clipboard")
                    flat: true

                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                }
            }

            Image {
                id: preview

                property real ratio: sourceSize.width / sourceSize.height

                anchors.centerIn: parent
                width: parent.width * 0.8
                height: width / ratio

                source: "image://screenshooter/last"
                fillMode: Image.Stretch
                cache: false

                layer.enabled: true
                layer.effect: FluidMaterial.ElevationEffect {
                    elevation: 2
                }
            }
        }
    }

    Timer {
        id: shootTimer
        interval: grabDelay.value * 1000
        onTriggered: {
            Screenshooter.takeScreenshot(optionCombo.currentIndex + 1,
                                         includePointer.enabled && includePointer.checked,
                                         includeBorder.enabled && includeBorder.checked);
        }
    }

    Timer {
        id: showTimer
        interval: 1000
        onTriggered: {
            window.pageStack.push(previewPage);
            window.show();
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Save Screenshot As")
        fileMode: FileDialog.SaveFile
        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
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
}
