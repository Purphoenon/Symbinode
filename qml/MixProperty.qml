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
    signal modeChanged(int mode)
    signal factorChanged(real f)
    signal includingAlphaChanged(bool including)
    signal propertyChangingFinished(string name, var newValue, var oldValue)

    ParamDropDown {
        id: control
        y: 15
        model: ["Mix", "Add", "Multiply", "Substract", "Divide"]
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

    CheckBox {
        id: includingAlphaParam
        y: 86
        leftPadding: 30
        height: 25
        width: 135
        text: qsTr("Including alpha")
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
