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
    height: childrenRect.height + 30
    width: parent.width
    property alias startInterpolation: control.currentIndex
    property alias startRadius: radiusParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startUseAlpha: useAlphaParam.checked
    property alias startBits: bitsParam.currentIndex
    signal interpolationChanged(int interpolation)
    signal radiusChanged(real radius)
    signal smoothValueChanged(real smooth)
    signal useAlphaChanged(bool use)
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
    ParamDropDown {
        id: control
        y: 53
        model: ["Linear", "Hermite"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                interpolationChanged(0)
            }
            else if(currentIndex == 1) {
                interpolationChanged(1)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startInterpolation", currentIndex, oldIndex)
        }
    }
    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 91
        clip: true
        ParamSlider {
            id: radiusParam
            propertyName: "Radius"
            onPropertyValueChanged: {
                radiusChanged(radiusParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startRadius", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: smoothParam
            y: 18
            propertyName: "Smooth"
            onPropertyValueChanged: {
                smoothValueChanged(smoothParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startSmooth", propertyValue, oldValue)
            }
        }
    }
    ParamCheckbox {
        id: useAlphaParam
        y: 157
        width: 90
        text: qsTr("Use alpha")
        checked: true
        onCheckedChanged: {
            useAlphaChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startUseAlpha", checked, !checked)
            focus = false
        }
    }
}
