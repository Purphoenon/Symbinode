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
    id: colorSlider
    property real value: (1 - pickerCursor.x/width)
    property real proportionX: 0
    signal hueChanged(real hueValue)
    signal hueChangingStarted()
    signal hueChangingFinished()
    width: 300; height: 15
    Item {
        id: pickerCursor
        height: parent.height
        x: proportionX * parent.width
        Rectangle {
            x: -7; y: 0
            width: 7*2; height: 7*2
            radius: 7
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
        x: -Math.round(7/2)
        width: parent.width + 7
        height: 14
        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                proportionX = Math.max(0, Math.min(1, mouse.x/parent.width))
                value = (1 - pickerCursor.x/parent.width)
            }
        }
        onPositionChanged: {
            handleMouse(mouse)
            hueChanged(colorSlider.value)
        }
        onPressed: {
            hueChangingStarted()
            handleMouse(mouse)
        }
        onReleased: hueChangingFinished()
    }
}
