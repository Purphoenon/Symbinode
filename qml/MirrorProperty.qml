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
import QtQuick.Controls 2.5

Item {
    id: mirrorProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startDirection: control.currentIndex
    property alias startBits: bitsParam.currentIndex
    signal directionChanged(int dir)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: bitsParam
        y: 15
        model: ["8 bits per channel", "16 bits per channel"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                bitsChanged(0)
            }
            else if(currentIndex == 1) {
                bitsChanged(1)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startBits", currentIndex, oldIndex)
        }
    }
    ParamDropDown {
        id: control
        y: 53
        model: ["Left to right", "Right to left", "Top to bottom", "Bottom to top"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                directionChanged(0)
            }
            else if(currentIndex == 1) {
                directionChanged(1)
            }
            else if(currentIndex == 2) {
                directionChanged(2)
            }
            else if(currentIndex == 3) {
                directionChanged(3)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startDirection", currentIndex, oldIndex)
        }
    }
}
