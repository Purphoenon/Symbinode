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
    property alias startSides: sidesParam.propertyValue
    property alias startScale: scaleParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startUseAlpha: useAlphaParam.checked
    signal sidesChanged(int sides)
    signal polygonScaleChanged(real scale)
    signal polygonSmoothChanged(real smooth)
    signal useAlphaChanged(bool use)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamSlider {
        id: sidesParam
        propertyName: "Sides"
        maximum: 36
        minimum: 3
        step: 1
        onPropertyValueChanged: {
            sidesChanged(sidesParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSides", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: scaleParam
        y: 33
        propertyName: "Scale"
        onPropertyValueChanged: {
            polygonScaleChanged(scaleParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScale", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: smoothParam
        y: 66
        propertyName: "Smooth"
        onPropertyValueChanged: {
            polygonSmoothChanged(smoothParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSmooth", propertyValue, oldValue)
        }
    }
    ParamCheckbox {
        id: useAlphaParam
        y: 114
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
