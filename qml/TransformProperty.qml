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
    CheckBox {
        id: clampParam
        y: 180
        leftPadding: 30
        height: 25
        width: 100
        text: qsTr("Clamp")
        checked: false
        onCheckedChanged: {
            clampCoordsChanged(clampParam.checked)
        }
        onToggled: {
            propertyChangingFinished("startClamp", checked, !checked)
            focus = false
        }

        indicator: Item {
                       implicitWidth: 30
                       implicitHeight: 30
                       x: clampParam.contentItem.width + 5
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
                               visible: clampParam.checked
                               color: "#A2A2A2"
                           }
                       }
                   }

        contentItem: Text {
            topPadding: 0
            text: clampParam.text
            color: "#A2A2A2"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            renderType: Text.NativeRendering
        }
    }
}
