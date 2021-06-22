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
    id: brightnessContrastProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startBrightness: brightnessParam.propertyValue
    property alias startContrast: contrastParam.propertyValue
    property alias startBits: control.currentIndex
    signal brightnessChanged(real value)
    signal contrastChanged(real value)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: control
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
    ParamSlider {
        id: brightnessParam
        y: 38
        propertyName: "Brightness"
        minimum: -1
        onPropertyValueChanged: {
            brightnessChanged(brightnessParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startBrightness", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: contrastParam
        y: 71
        propertyName: "Contrast"
        minimum: -1
        onPropertyValueChanged: {
            contrastChanged(contrastParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startContrast", propertyValue, oldValue)
        }
    }
}
