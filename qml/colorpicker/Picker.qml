/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.12

Item {
    property int r : 6
    property color hueColor : "blue"
    property real saturation : proportionX
    property real brightness : 1 - proportionY
    property real proportionX: 0
    property real proportionY: 0
    signal sbChanged(real s, real b)
    width: 250; height: 120
    clip: true

    Rectangle {
        width: parent.height
        height: parent.width
        transform: Rotation { origin.x: 0; origin.y: 0; angle: 270}
        y: width
        gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#FFFFFFFF"
            }
            GradientStop {
                position: 1.0; color: hueColor
            }
        }
    }
    Rectangle {
        width: parent.width
        height: parent.height
        gradient: Gradient {
            GradientStop {
                position: 1.0; color: "#FF000000"
            }
            GradientStop {
                position: 0.0; color: "#00000000"
            }
        }
    }

    Item {
        width: parent.width
        height: parent.height
        x: r
        y: r

        Item {
            id: pickerCursor
            x: proportionX * parent.width - r
            y: proportionY * parent.height - r
            Rectangle {
                x: -r; y: -r
                width: r*2; height: r*2
                radius: r
                border.color: "black"; border.width: 1
                color: "transparent"
                Rectangle {
                    anchors.fill: parent; anchors.margins: 1;
                    border.color: "white"; border.width: 1
                    radius: width/2
                    color: "transparent"
                }
            }
        }
        MouseArea {
            x: -r
            y: -r
            width: parent.width + r
            height: parent.height + r
            function handleMouse(mouse) {
                if (mouse.buttons & Qt.LeftButton) {
                    proportionX = Math.max(0, Math.min(1,  mouse.x/(parent.width)));
                    proportionY = Math.max(0, Math.min(1, mouse.y/parent.height));
                    saturation = proportionX
                    brightness = 1 - proportionY
                }
            }
            onPositionChanged: {
                handleMouse(mouse)
                sbChanged(saturation, brightness)
            }
            onPressed: handleMouse(mouse)
        }
    }
}
