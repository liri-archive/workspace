/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
import QtQuick.Dialogs 1.2
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

ApplicationWindow {
    id: root
    visible: true

    ColumnLayout {
        anchors {
            fill: parent
            margins: Themes.Units.largeSpacing
        }

        Keys.onEscapePressed: Qt.quit()

        GroupBox {
            title: qsTr("Take Screenshot")

            ColumnLayout {
                spacing: Themes.Units.smallSpacing

                Components.Icon {
                    iconName: "applets-screenshooter"
                }

                ColumnLayout {
                    spacing: Themes.Units.smallSpacing

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
        }

        RowLayout {
            spacing: Themes.Units.smallSpacing

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Cancel")
                onClicked: Qt.quit()
            }

            Button {
                text: qsTr("Take Screenshot")
                onClicked: {
                    root.hide();
                    shootTimer.start();
                }
            }
        }
    }

    Timer {
        id: shootTimer
        interval: grabDelay.value * 1000
        onTriggered: Screenshooter.takeScreenshot(options.current.what)
    }

    Timer {
        id: showTimer
        interval: 1000
        onTriggered: root.show()
    }

    Connections {
        target: Screenshooter
        onScreenshotDone: showTimer.start()
    }
}
