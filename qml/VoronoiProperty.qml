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
    property alias startScale: scaleParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startJitter: jitterParam.propertyValue
    property alias startIntensity: intensityParam.propertyValue
    property alias startSeed: seedParam.propertyValue
    property alias startBorders: bordersParam.propertyValue
    property alias startInverse: inverseParam.checked
    property alias type: control.currentIndex
    property alias startBits: bitsParam.currentIndex
    signal voronoiTypeChanged(string type)
    signal voronoiScaleChanged(int scale)
    signal scaleXChanged(int scale)
    signal scaleYChanged(int scale)
    signal jitterChanged(real jitter)
    signal inverseChanged(bool inverse)
    signal intensityChanged(real intensity)
    signal bordersChanged(real size)
    signal seedChanged(int seed)
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
    ParamDropDown{
        id: control
        y: 53
        model: ["Crystals", "Borders", "Solid", "Worley"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                voronoiTypeChanged("crystals")
            }
            else if(currentIndex == 1) {
                voronoiTypeChanged("borders")
            }
            else if(currentIndex == 2) {
                voronoiTypeChanged("solid")
            }
            else if(currentIndex == 3) {
                voronoiTypeChanged("worley")
            }
            focus = false
        }

        onActivated: {
            propertyChangingFinished("type", currentIndex, oldIndex)
        }
    }

    ParamSlider {
        id: scaleParam
        y: 76
        propertyName: "Scale"
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            voronoiScaleChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScale", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 109
        propertyName: "Scale X"
        maximum: 20
        minimum: 1
        step: 1
        onPropertyValueChanged: {
            scaleXChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScaleX", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 142
        propertyName: "Scale Y"
        maximum: 20
        minimum: 1
        step: 1
        onPropertyValueChanged: {
            scaleYChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startScaleY", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: jitterParam
        y: 175
        propertyName: "Jitter"
        onPropertyValueChanged: {
            jitterChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startJitter", propertyValue, oldValue)
        }
    }
    ParamCheckbox{
        id: inverseParam
        y: control.currentIndex == 1 ? 322 : 289
        width: 80
        text: qsTr("Inverse")
        onCheckedChanged: {
            inverseChanged(inverseParam.checked)
        }
        onToggled: {
            propertyChangingFinished("startInverse", checked, !checked)
            focus = false
        }
    }
    ParamSlider {
        id: intensityParam
        y: 208
        maximum: 2
        propertyName: "Intensity"
        onPropertyValueChanged: {
            intensityChanged(intensityParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startIntensity", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: seedParam
        y: 241
        minimum: 1
        maximum: 100
        step: 1
        propertyName: "Seed"
        onPropertyValueChanged: {
            seedChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSeed", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: bordersParam
        y: 274
        visible: control.currentIndex == 1
        propertyName: "Width"
        onPropertyValueChanged: {
            bordersChanged(bordersParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startBorders", propertyValue, oldValue)
        }
    }
}
