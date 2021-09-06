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
    id: mappingProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startInputMin: inputMinParam.propertyValue
    property alias startInputMax: inputMaxParam.propertyValue
    property alias startOutputMin: outputMinParam.propertyValue
    property alias startOutputMax: outputMaxParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal inputMinChanged(real value)
    signal inputMaxChanged(real value)
    signal outputMinChanged(real value)
    signal outputMaxChanged(real value)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: bitsParam
        y: 15
        model: ["8 bits", "16 bits"]
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
    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 53
        clip: true
        ParamSlider {
            id: inputMinParam
            propertyName: "Input Min"
            onPropertyValueChanged: {
                inputMinChanged(inputMinParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startInputMin", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: inputMaxParam
            y: 18
            propertyName: "Input Max"
            onPropertyValueChanged: {
                inputMaxChanged(inputMaxParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startInputMax", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: outputMinParam
            y: 51
            propertyName: "Output Min"
            onPropertyValueChanged: {
                outputMinChanged(outputMinParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startOutputMin", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: outputMaxParam
            y: 84
            propertyName: "Output Max"
            onPropertyValueChanged: {
                outputMaxChanged(outputMaxParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startOutputMax", propertyValue, oldValue)
            }
        }
    }
}
