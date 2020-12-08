import QtQuick 2.12

Item {
    property var activeItem
    property var gradientStops: gradientsData()
    signal gradientStopAdded(vector3d color, real pos)
    signal positionChanged(real pos, int index)
    signal colorChanged(vector3d color, int index)
    signal gradientStopDeleted(int index)
    signal gradientsStopsChanged(var gradients)
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

    onGradientsStopsChanged: {
        grad.stops = []
        for(var i = 0; i < gradientContainer.pointers.length; ++i) {
            gradientContainer.pointers[i].destroy()
        }
        gradientContainer.pointers = []
        for(var i = 0; i < gradients.length; ++i) {
            createGradientPointer(gradients[i][3], Qt.rgba(gradients[i][0],gradients[i][1],gradients[i][2], 1))
        }
    }

    Keys.onPressed: {
        if(event.key == Qt.Key_Delete) {
            if(grad.stops.length < 2) return
            var pos = activeItem.proportionX
            var gradStop
            var index
            var newStops = []
            for(var i = 0; i < grad.stops.length; ++i) {
                if(pos === grad.stops[i].position && activeItem.gradientColor === grad.stops[i].color) {
                    gradStop = grad.stops[i]
                    index = i
                }
                else {
                    newStops.push(grad.stops[i])
                }
            }
            grad.stops = newStops
            gradStop.destroy()
            gradientContainer.pointers.splice(gradientContainer.pointers.indexOf(activeItem), 1)
            activeItem.destroy()
            activeItem = null
            gradientItem.update()
            gradientStopDeleted(index)
            gradientStops = gradientsData()
        }
    }

    function setActiveItem(item) {
        if(activeItem) activeItem.selected = false
        activeItem = item
        activeItem.selected = true
    }
    function gradientPositionUpdate() {
        var pos = activeItem.proportionX
        var index
        for(var i = 0; i < grad.stops.length; ++i) {
            if(pos === grad.stops[i].position && activeItem.gradientColor === grad.stops[i].color) {
                index = i
            }
        }
        positionChanged(pos, index)
        gradientStops = gradientsData()
    }
    function gradientColorUpdate() {
        var pos = activeItem.proportionX
        var index
        for(var i = 0; i < grad.stops.length; ++i) {
            if(pos === grad.stops[i].position && activeItem.gradientColor === grad.stops[i].color) {
                index = i
            }
        }
        colorChanged(Qt.vector3d(activeItem.gradientColor.r, activeItem.gradientColor.g, activeItem.gradientColor.b), index)
        gradientStops = gradientsData()
    }

    function createGradientPointer(pos, color) {
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
            var gradientStopObject = Qt.createQmlObject('import QtQuick 2.12; GradientStop {}',
                                               grad,
                                               "ColorRampProperty.qml");
            gradientStopObject.position = Qt.binding(function(){return gradientPointerObject.proportionX})
            gradientStopObject.color = Qt.binding(function(){return gradientPointerObject.gradientColor})
            grad.stops.push(gradientStopObject)
            gradientContainer.pointers.push(gradientPointerObject)
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
        y: 15
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
                    createGradientPointer(p, c)
                }
                else if(!leftStop) {
                    c = rightStop.color
                    createGradientPointer(p, c)
                }
                else if(!rightStop) {
                    c = leftStop.color
                    createGradientPointer(p, c)
                }
                gradientStopAdded(Qt.vector3d(c.r, c.g, c.b), p)
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
