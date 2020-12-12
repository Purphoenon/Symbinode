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

Item {
    property real from: 0.0
    property real to: 1.0
    property real step: 0.01
    property real value: 0
    property alias text: textValue.text
    signal newValueChanged(real value)

    onValueChanged: {
        text = value
    }

    width: 20
    height: 24

    TextInput {
        property string oldText: value
        id: textValue
        x: parent.width - 50
        topPadding: 0
        y: 5
        width: 20
        height: 15
        activeFocusOnPress: false
        color: "#D8D9D9"
        text: value
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 8
        selectByMouse: true
        onAccepted: {
            focus = false
        }
        onFocusChanged: {
            var newValue = Number(text)
            text = Math.max(Math.min(maximum, newValue), minimum)
            value = text
            newValueChanged(value)
        }

        onTextEdited: {
            if(text.length < oldText.length) {
                oldText = text
                return
            }
            var index
            if(text.lastIndexOf(".") !== -1) {
                index = text.lastIndexOf(".")
                if(text.length > index + 3) {
                    text = text.substring(0, index + 3)
                    return
                }
            }

            var simbol = text[text.length - 1]
            for(var i = 0; i < oldText.length; ++i) {
                if(oldText[i] !== text[i]) {
                    simbol = text[i]
                    index = i
                    break
                }
            }

            var newText
            var newValue
            var pointIndex = text.lastIndexOf(".")
            var isRealNumber = pointIndex !== -1
            pointIndex = isRealNumber ? pointIndex : text.length
            var maxLenght = String(maximum).length
            if(minimum < 0) maxLenght += 1
            if(text.substring(0, pointIndex).length > maxLenght) {
                text = oldText
                return
            }

            if(simbol === "0") {
                var isWholePart = isRealNumber ? pointIndex > index : true
                if(isWholePart) {
                    var wholePart = text.substring(0, pointIndex)
                    if((wholePart.match(/0/g) || []).length > 1 && Number(wholePart) == 0) {
                        text = oldText
                        return
                    }
                }
            }
            if(Number.isInteger(parseInt(simbol)) ||
              ((simbol === ".") && (step < 1) && (text.substring(0, text.length-1).lastIndexOf(".") === -1))){
                newText = text
                newValue = Number(text)
                var n = (newValue/step).toFixed(2)

                if((n - Math.trunc(n) != 0)) {
                    newText = oldText
                }
            }
            else if((simbol === "-") && (minimum < 0) && text.lastIndexOf("-") === 0) {
                return
            }

            else {
                newText = oldText
            }
            text = newText
            oldText = text
        }
    }

    MouseArea {
        x: parent.width - 50
        y: 5
        width: 20
        height: 15

        onDoubleClicked: {
            textValue.focus = true
        }
    }

    MouseArea {
        property bool hovered: false
        id: upValue
        x: textValue.x + textValue.width + 5
        y: 1
        hoverEnabled: true
        width: 6
        height: parent.height/2 - 2
        onEntered: {
            hovered = true
            upCanvas.requestPaint()
        }
        onExited: {
            hovered = false
            upCanvas.requestPaint()
        }

        Canvas {
            id: upCanvas
            width: 6
            height: 4
            y: 5
            contextType: "2d"
            onPaint: {
                var ctx = getContext("2d")
                ctx.reset();
                ctx.fillStyle = upValue.hovered ? "#A2A2A2" : "#4DA2A2A2"
                ctx.lineTo(6, 4)
                ctx.lineTo(3, 0)
                ctx.lineTo(0, 4)
                ctx.fill()
            }
        }
        onClicked: {
            var newValue = value + step
            value = Math.min(newValue, to).toFixed(2)
            newValueChanged(value)
        }
    }

    MouseArea {
        property bool hovered: false
        id: downValue
        x: textValue.x + textValue.width + 5
        y: parent.height - height
        width: 6
        height: parent.height/2 - 2
        hoverEnabled: true

        onEntered: {
            hovered = true
            downCanvas.requestPaint()
        }

        onExited: {
            hovered = false
            downCanvas.requestPaint()
        }

        Canvas {
            id: downCanvas
            width: 6
            height: 4
            y: 0.5
            contextType: "2d"
            onPaint: {
                var context = getContext("2d")
                context.reset();
                context.fillStyle = downValue.hovered ? "#A2A2A2" : "#4DA2A2A2"
                context.moveTo(0, 0)
                context.lineTo(6, 0)
                context.lineTo(3, 4)
                context.closePath()
                context.fill()
            }
        }
        onClicked: {
            var newValue = value - step
            value = Math.max(newValue, from).toFixed(2)
            newValueChanged(value)
        }
    }
}
