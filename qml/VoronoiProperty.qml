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
    signal voronoiTypeChanged(string type)
    signal voronoiScaleChanged(int scale)
    signal scaleXChanged(int scale)
    signal scaleYChanged(int scale)
    signal jitterChanged(real jitter)
    signal inverseChanged(bool inverse)
    signal intensityChanged(real intensity)
    signal bordersChanged(real size)
    signal seedChanged(int seed)

    ParamDropDown{
        id: control
        y: 15
        model: ["Crystals", "Borders", "Solid", "Worley"]
        onActivated: {
            if(index == 0) {
                voronoiTypeChanged("crystals")
            }
            else if(index == 1) {
                voronoiTypeChanged("borders")
            }
            else if(index == 2) {
                voronoiTypeChanged("solid")
            }
            else if(index == 3) {
                voronoiTypeChanged("worley")
            }
            focus = false
        }
    }

    ParamSlider {
        id: scaleParam
        y: 38
        propertyName: "Scale"
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            voronoiScaleChanged(propertyValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 71
        propertyName: "Scale X"
        maximum: 20
        minimum: 1
        step: 1
        onPropertyValueChanged: {
            scaleXChanged(propertyValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 104
        propertyName: "Scale Y"
        maximum: 20
        minimum: 1
        step: 1
        onPropertyValueChanged: {
            scaleYChanged(propertyValue)
        }
    }
    ParamSlider {
        id: jitterParam
        y: 137
        propertyName: "Jitter"
        onPropertyValueChanged: {
            jitterChanged(propertyValue)
        }
    }
    CheckBox {
        id: inverseParam
        y: control.currentIndex == 1 ? 284 : 251
        leftPadding: 30
        height: 25
        width: 100
        text: qsTr("Inverse")
        checked: false        
        onCheckedChanged: {
            inverseChanged(inverseParam.checked)
            focus = false
        }

        indicator: Item {
                       implicitWidth: 30
                       implicitHeight: 30
                       x: inverseParam.contentItem.width + 5
                       anchors.verticalCenter: parent.verticalCenter
                       Rectangle {
                           width: 14
                           height: 14
                           anchors.centerIn: parent
                           color: "transparent"
                           border.color: "#A2A2A2"
                           Rectangle {
                               width: 6
                               height: 6
                               anchors.centerIn: parent
                               visible: inverseParam.checked
                               color: "#A2A2A2"
                           }
                       }
                   }

        contentItem: Text {
            topPadding: 0
            text: inverseParam.text
            color: "#A2A2A2"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            renderType: Text.NativeRendering
        }
    }

    ParamSlider {
        id: intensityParam
        y: 170
        maximum: 2
        propertyName: "Intensity"
        onPropertyValueChanged: {
            intensityChanged(intensityParam.propertyValue)
        }
    }

    ParamSlider {
        id: seedParam
        y: 203
        minimum: 1
        maximum: 100
        step: 1
        propertyName: "Seed"
        onPropertyValueChanged: {
            seedChanged(propertyValue)
        }
    }

    ParamSlider {
        id: bordersParam
        y: 236
        visible: control.currentIndex == 1
        propertyName: "Width"
        onPropertyValueChanged: {
            bordersChanged(bordersParam.propertyValue)
        }
    }
}
