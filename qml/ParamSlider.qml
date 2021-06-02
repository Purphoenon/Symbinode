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
    property alias minimum: slider.from
    property alias maximum: slider.to
    property alias step: slider.stepSize
    property alias propertyName: label.text
    property alias propertyValue: slider.value
    property bool interacted: slider.pressed
    property real oldValue
    signal changingFinished()

    width: parent.width
    height: childrenRect.height
    Label {
       id: label
       height: 25
       width: 100
       verticalAlignment: Text.AlignVCenter
       text: "Размер"
       color: "#D8D9D9"
       x: 30
       y: 15
       z: 1
    }

    Slider {
        id: slider
        x: 5
        y: 15
        width: parent.width - 10
        height: 25
        clip: true
        stepSize: 0.01
        onValueChanged: {
            valueBox.value = value.toFixed(2)
            focus = false
        }
        onPressedChanged: {
            if(pressed) oldValue = value
            if(!pressed && (oldValue != value)) changingFinished()
        }

        Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: slider.availableWidth
            height: 25
            clip: true
            z: 1
            color: "transparent"
            Canvas {
                id: mycanvas
                x: slider.visualPosition * (slider.availableWidth) - 2
                y: slider.topPadding + slider.availableHeight / 2 - parent.height / 2
                width: 4
                height: 3
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.fillStyle = Qt.rgba(0.84, 0.84, 0.84, 1);
                    ctx.lineTo(4, 0)
                    ctx.lineTo(2, 3)
                    ctx.lineTo(0, 0)
                    ctx.fill()
                }
            }
        }

        handle: Item{
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: 4
            height: 25
            Rectangle {
                visible: (parent.x > 3 + slider.leftPadding) && (parent.x < slider.availableWidth - 3 +
                                                                 slider.leftPadding)
                x: -3
                width: 3
                height: 25
                color: "#57758F"
            }
        }

        background:
            Rectangle {
                    x: slider.leftPadding
                    y: slider.topPadding + slider.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 25
                    width: slider.availableWidth
                    height: implicitHeight
                    color: "#484C51"
                    radius: 3

                    Rectangle {
                        width: slider.visualPosition * parent.width
                        height: parent.height
                        color: "#57758F"
                        radius: 3
                    }
                }
        from: 0
        to: 1
    }

    StepValueBox {
        id: valueBox
        x: parent.width - 25
        y: 15
        width: 20
        height: 25
        from: slider.from
        to: slider.to
        step: slider.stepSize
        value: slider.value
        onNewValueChanged: {
            if(slider.value == value) return
            oldValue = slider.value
            slider.value = value
            changingFinished()
        }
    }
}
