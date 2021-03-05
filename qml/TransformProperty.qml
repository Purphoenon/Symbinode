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
    property alias startTransX: transXParam.propertyValue
    property alias startTransY: transYParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startRotation: rotationParam.propertyValue
    property alias startClamp: clampParam.checked
    signal transXChanged(real x)
    signal transYChanged(real y)
    signal scaleXChanged(real x)
    signal scaleYChanged(real y)
    signal angleChanged(int angle)
    signal clampCoordsChanged(bool clamp)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamSlider {
        id: transXParam
        propertyName: "Translate X"
        minimum: -1
        onPropertyValueChanged: {
            transXChanged(transXParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startTransX", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: transYParam
        y: 33
        propertyName: "Translate Y"
        minimum: -1
        onPropertyValueChanged: {
            transYChanged(transYParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startTransY", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 66
        propertyName: "Scale X"
        maximum: 2
        onPropertyValueChanged: {
            scaleXChanged(scaleXParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScaleX", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 99
        propertyName: "Scale Y"
        maximum: 2
        onPropertyValueChanged: {
            scaleYChanged(scaleYParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScaleY", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: rotationParam
        y: 132
        propertyName: "Rotation"
        maximum: 360
        step: 1
        onPropertyValueChanged: {
            angleChanged(rotationParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startRotation", propertyValue, oldValue)
        }
    }
    ParamCheckbox {
        id: clampParam
        y: 180
        width: 75
        text: qsTr("Clamp")
        onCheckedChanged: {
            clampCoordsChanged(clampParam.checked)
        }
        onToggled: {
            propertyChangingFinished("startClamp", checked, !checked)
            focus = false
        }
    }
}
