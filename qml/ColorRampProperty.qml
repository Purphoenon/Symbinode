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
    property var activeItem
    property var operation
    property var gradientStops: gradientsData()
    property alias startBits: control.currentIndex
    signal gradientStopAdded(vector3d color, real pos, int index)
    signal positionChanged(real pos, int index)
    signal colorChanged(vector3d color, int index)
    signal gradientStopDeleted(int index)
    signal gradientsStopsChanged(var gradients)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    height: childrenRect.height + 30
    width: parent.width

    MouseArea {
        width: parent.width
        height: parent.parent.parent.height - 30
        hoverEnabled: true
        onEntered: {
            parent.focus = true
        }
        onExited: {
            parent.focus = false
        }
    }

    ParamDropDown {
        id: control
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

    onOperationChanged: {
        var command = operation[0]
        switch(command) {
        case 0: //add
            var gradientColor = operation[1]
            var gradientPos = operation[2]
            var index = operation[3]
            createGradientPointer(gradientPos, gradientColor, index)
            gradientStopAdded(Qt.vector3d(gradientColor.r, gradientColor.g, gradientColor.b), gradientPos, index)
            break
        case 1: //change pos
            var pos = operation[1]
            var index = operation[2]
            var gradientPointer = gradientContainer.pointers[index]
            setActiveItem(gradientPointer)
            gradientPointer.proportionX = pos
            break
        case 2: //change color
            var color = operation[1]
            var index = operation[2]
            var gradientPointer = gradientContainer.pointers[index]
            setActiveItem(gradientPointer)
            gradientPointer.gradientColor = color
            break
        case 3:  //delete
            var index = operation[1]
            var gradStop
            var gradPointer = gradientContainer.pointers[index]
            setActiveItem(gradPointer)
            var newStops = []
            for(var i = 0; i < grad.stops.length; ++i) {
                if(gradPointer.proportionX === grad.stops[i].position && gradPointer.gradientColor === grad.stops[i].color) {
                    gradStop = grad.stops[i]
                }
                else {
                    newStops.push(grad.stops[i])
                }
            }
            grad.stops = newStops
            gradStop.destroy()            
            gradientContainer.pointers.splice(index, 1)
            gradPointer.destroy()
            activeItem = null
            gradientItem.update()
            gradientStops = gradientsData()
            gradientStopDeleted(index)
            break
        default:
            console.log("unknown operation")
            break
        }        
    }

    onGradientsStopsChanged: {
        grad.stops = []
        for(var i = 0; i < gradientContainer.pointers.length; ++i) {
            gradientContainer.pointers[i].destroy()
        }
        gradientContainer.pointers = []
        for(var i = 0; i < gradients.length; ++i) {
            createGradientPointer(gradients[i][3], Qt.rgba(gradients[i][0],gradients[i][1],gradients[i][2], 1), i)
        }
    }

    Keys.onPressed: {
        if(event.key == Qt.Key_Delete) {
            if(grad.stops.length < 2) return
            var pos = activeItem.proportionX
            var index = gradientContainer.pointers.indexOf(activeItem)

            var newValue = [3]
            newValue.push(index)
            var oldValue = [0]
            var c = activeItem.gradientColor
            oldValue.push(Qt.rgba(c.r, c.g, c.b, 1.0))
            oldValue.push(pos)
            oldValue.push(index)
            propertyChangingFinished("operation", newValue, oldValue)
        }
    }

    function setActiveItem(item) {
        if(activeItem) activeItem.selected = false
        activeItem = item
        activeItem.selected = true
    }
    function gradientPositionUpdate() {
        var pos = activeItem.proportionX
        var index = gradientContainer.pointers.indexOf(activeItem)
        positionChanged(pos, index)
        gradientStops = gradientsData()
    }
    function gradientPositionChanged() {
        var newValue = [1]
        var index = gradientContainer.pointers.indexOf(activeItem)
        newValue.push(activeItem.proportionX)
        newValue.push(index)
        var oldValue = [1]
        oldValue.push(activeItem.oldPos)
        oldValue.push(index)
        propertyChangingFinished("operation", newValue, oldValue)
    }

    function gradientColorUpdate() {
        var pos = activeItem.proportionX
        var index = gradientContainer.pointers.indexOf(activeItem)
        colorChanged(Qt.vector3d(activeItem.gradientColor.r, activeItem.gradientColor.g, activeItem.gradientColor.b), index)
        gradientStops = gradientsData()
    }

    function gradientColorChanged() {
        var newValue = [2]
        var index = gradientContainer.pointers.indexOf(activeItem)
        newValue.push(Qt.rgba(activeItem.gradientColor.r, activeItem.gradientColor.g, activeItem.gradientColor.b, 1))
        newValue.push(index)
        var oldValue = [2]
        oldValue.push(Qt.rgba(activeItem.oldColor.r, activeItem.oldColor.g, activeItem.oldColor.b, 1))
        oldValue.push(index)
        propertyChangingFinished("operation", newValue, oldValue)
    }

    function createGradientPointer(pos, color, index) {
        var gradientPointerComponent = Qt.createComponent("GradientPointer.qml")
        if(gradientPointerComponent.status == Component.Ready) {
            var gradientPointerObject = gradientPointerComponent.createObject(gradientContainer)
            gradientPointerObject.gradientColor = color
            gradientPointerObject.proportionX = pos
            gradientPointerObject.y = gradientContainer.height - gradientPointerObject.height*0.5
            gradientPointerObject.z = 1
            gradientPointerObject.activated.connect(setActiveItem)
            gradientPointerObject.proportionXChanged.connect(gradientPositionUpdate)
            gradientPointerObject.gradientColorChanged.connect(gradientColorUpdate)
            gradientPointerObject.posChangingFinished.connect(gradientPositionChanged)
            gradientPointerObject.colorChangingFinished.connect(gradientColorChanged)
            var gradientStopObject = Qt.createQmlObject('import QtQuick 2.12; GradientStop {}',
                                               grad,
                                               "ColorRampProperty.qml");
            gradientStopObject.position = Qt.binding(function(){return gradientPointerObject.proportionX})
            gradientStopObject.color = Qt.binding(function(){return gradientPointerObject.gradientColor})
            grad.stops.push(gradientStopObject)
            gradientContainer.pointers.splice(index, 0, gradientPointerObject)
            setActiveItem(gradientPointerObject)
            gradientStops = gradientsData()
        }
    }

    function gradientsData() {
        var data = []
        for(var i = 0; i < grad.stops.length; ++i) {
            var g = grad.stops[i]
            var d = []
            d.push(g.color.r)
            d.push(g.color.g)
            d.push(g.color.b)
            d.push(g.position)
            data.push(d)
        }
        return data
    }

    Item {
        property var pointers: []
        id: gradientContainer
        x: 10
        y: 53
        width: parent.width - 20
        height: 25
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {
                var gr = gradientItem.gradient
                var leftBorder = 0
                var rightBorder = 1
                var leftStop
                var rightStop
                var p = mouse.x/width
                for(var i = 0; i < gr.stops.length; ++i) {
                    var curP = gr.stops[i].position
                    if((curP >= leftBorder) && (curP < p)) {
                        leftBorder = curP
                        leftStop = gr.stops[i]
                    }
                    else if((curP <= rightBorder) && (curP > p)) {
                        rightBorder = curP
                        rightStop = gr.stops[i]
                    }
                }

                var c
                if(leftStop && rightStop) {
                    var k = (p - leftStop.position)/(rightStop.position - leftStop.position)
                    var r = leftStop.color.r*(1.0 - k) + rightStop.color.r*k
                    var g = leftStop.color.g*(1.0 - k) + rightStop.color.g*k
                    var b = leftStop.color.b*(1.0 - k) + rightStop.color.b*k
                    c = Qt.rgba(r, g, b, 1)
                }
                else if(!leftStop) {
                    c = rightStop.color
                }
                else if(!rightStop) {
                    c = leftStop.color
                }

                var newValue = [0]
                newValue.push(c)
                newValue.push(p)
                newValue.push(gr.stops.length)
                var oldValue = [3]
                oldValue.push(gr.stops.length)
                propertyChangingFinished("operation", newValue, oldValue)
            }
        }
        Rectangle {
            id: gradientItem
            width: parent.width
            height: parent.height
            radius: 3

            gradient: Gradient {
                id: grad
                orientation: Gradient.Horizontal
            }
        }
    }
}
