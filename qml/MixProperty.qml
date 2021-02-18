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
    property alias startMode: control.currentIndex
    property alias startincludingAlpha: includingAlphaParam.checked
    property real startFactor
    property int startForegroundOpacity
    property int startBackgroundOpacity
    signal modeChanged(int mode)
    signal factorChanged(real f)
    signal includingAlphaChanged(bool including)
    signal foregroundOpacityChanged(int opacity);
    signal backgroundOpacityChanged(int opacity);
    signal propertyChangingFinished(string name, var newValue, var oldValue)

    ParamDropDown {
        id: control
        y: 15
        model: ["Normal", "Mix", "Overlay", "Screen", "Soft-light", "Hard-light", "Lighten", "Color-dodge", "Color-burn",
                "Darken", "Add", "Subtract", "Multiply", "Divide", "Difference", "Exclusion"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                modeChanged(0)
            }
            else if(currentIndex == 1) {
                modeChanged(1)
            }
            else if(currentIndex == 2) {
                modeChanged(2)
            }
            else if(currentIndex == 3) {
                modeChanged(3)
            }
            else if(currentIndex == 4) {
                modeChanged(4)
            }
            else if(currentIndex == 5) {
                modeChanged(5)
            }
            else if(currentIndex == 6) {
                modeChanged(6)
            }
            else if(currentIndex == 7) {
                modeChanged(7)
            }
            else if(currentIndex == 8) {
                modeChanged(8)
            }
            else if(currentIndex == 9) {
                modeChanged(9)
            }
            else if(currentIndex == 10) {
                modeChanged(10)
            }
            else if(currentIndex == 11) {
                modeChanged(11)
            }
            else if(currentIndex == 12) {
                modeChanged(12)
            }
            else if(currentIndex == 13) {
                modeChanged(13)
            }
            else if(currentIndex == 14) {
                modeChanged(14)
            }
            else if(currentIndex == 15) {
                modeChanged(15)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startMode", currentIndex, oldIndex)
        }
    }

    ParamSlider {
        id: factorParam
        y: 38
        propertyName: "Factor"
        propertyValue: startFactor
        onPropertyValueChanged: {
            factorChanged(factorParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startFactor", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: foregroundOpacityParam
        y: 71
        visible: control.currentIndex != 1
        propertyName: "Foreground opacity"
        propertyValue: startForegroundOpacity
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            foregroundOpacityChanged(foregroundOpacityParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startForegroundOpacity", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: backgroundOpacityParam
        y: 104
        visible: control.currentIndex != 1
        propertyName: "Background opacity"
        propertyValue: startBackgroundOpacity
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            backgroundOpacityChanged(backgroundOpacityParam.propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startBackgroundOpacity", propertyValue, oldValue)
        }
    }

    CheckBox {
        id: includingAlphaParam
        y: control.currentIndex != 1 ? 152 : 86
        leftPadding: 30
        height: 25
        width: 135
        text: qsTr("Use alpha")
        checked: true
        onCheckedChanged: {
            includingAlphaChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startIncludingAlpha", checked, !checked)
            focus = false
        }

        indicator: Item {
                       implicitWidth: 30
                       implicitHeight: 30
                       x: includingAlphaParam.contentItem.width + 5
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
                               visible: includingAlphaParam.checked
                               color: "#A2A2A2"
                           }
                       }
                   }

        contentItem: Text {
            topPadding: 0
            text: includingAlphaParam.text
            color: "#A2A2A2"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            renderType: Text.NativeRendering
        }
    }
}
