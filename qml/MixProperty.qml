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
    property alias startincludingAlpha: useAlphaParam.checked
    property real startFactor
    property int startForegroundOpacity
    property int startBackgroundOpacity
    property alias startBits: bitsParam.currentIndex
    signal modeChanged(int mode)
    signal factorChanged(real f)
    signal includingAlphaChanged(bool including)
    signal foregroundOpacityChanged(int opacity);
    signal backgroundOpacityChanged(int opacity);
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

    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 91
        clip: true
        ParamSlider {
            id: factorParam
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
            y: 18
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
            y: 51
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
    }

    ParamCheckbox {
        id: useAlphaParam
        y: control.currentIndex != 1 ? 190 : 126
        width: 90
        text: qsTr("Use alpha")
        checked: true
        onCheckedChanged: {
            includingAlphaChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startIncludingAlpha", checked, !checked)
            focus = false
        }
    }
}
