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
    width: parent.width
    height: childrenRect.height + 45
    property alias startNoiseScale: scaleParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startLayers: layersParam.propertyValue
    property alias startPersistence: persistenceParam.propertyValue
    property alias startAmplitude: amplitudeParam.propertyValue
    property alias startSeed: seedParam.propertyValue
    property alias type: control.currentIndex
    property alias startBits: bitsParam.currentIndex
    signal noiseTypeChanged(string type)
    signal noiseScaleChanged(real scale)
    signal scaleXChanged(real scale)
    signal scaleYChanged(real scale)
    signal layersChanged(int layers)
    signal persistenceChanged(real persistence)
    signal amplitudeChanged(real aplitude)
    signal seedChanged(int seed)
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
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                noiseTypeChanged("noisePerlin")
            }
            else if(currentIndex == 1) {
                noiseTypeChanged("noiseSimple")
            }
            focus = false
        }

        onActivated: {
            propertyChangingFinished("type", currentIndex, oldIndex)
        }
    }

    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 91
        clip: true
        ParamSlider {
            id: scaleParam
            propertyName: "Scale"
            maximum: 100
            step: 1
            onPropertyValueChanged: {
                noiseScaleChanged(scaleParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startNoiseScale", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: scaleXParam
            y: 18
            propertyName: "Scale X"
            step: 1
            minimum: 1
            maximum: 20
            onPropertyValueChanged: {
                scaleXChanged(scaleXParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startScaleX", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: scaleYParam
            y: 51
            propertyName: "Scale Y"
            step: 1
            minimum: 1
            maximum: 20
            onPropertyValueChanged: {
                scaleYChanged(scaleYParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startScaleY", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: layersParam
            y: 84
            propertyName: "Layers"
            maximum: 16
            step: 1
            onPropertyValueChanged: {
                layersChanged(layersParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startLayers", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: persistenceParam
            y: 117
            propertyName: "Persistence"
            onPropertyValueChanged: {
                persistenceChanged(persistenceParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startPersistence", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: amplitudeParam
            y: 150
            propertyName: "Amplitude"
            onPropertyValueChanged: {
                amplitudeChanged(amplitudeParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startAmplitude", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: seedParam
            y: 183
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
    }
}
