import QtQuick 2.12

Item {
    property alias gradientColor: colorPointer.color
    property real proportionX: 0.0
    property bool selected: false
    signal activated(var item)
    id: pointer
    x: parent.width * proportionX - width*0.5
    width: 10
    height: 20
    z: 1
    onSelectedChanged: {
        mycanvas.requestPaint()
    }

    Canvas {
        id: mycanvas
        width: 10
        height: 10
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = selected ? Qt.rgba(0.5, 0.5, 0.5, 1) : Qt.rgba(0.84, 0.84, 0.84, 1);
            ctx.lineTo(0, 10)
            ctx.lineTo(5, 1.34)
            ctx.lineTo(10, 10)
            ctx.fill()
        }
    }
    Rectangle {
        id: colorPointer
        width: 10
        height: 10
        y: 10
        color: "#000000"
        border.width: 1
        border.color: selected ? Qt.rgba(0.5, 0.5, 0.5, 1) : Qt.rgba(0.84, 0.84, 0.84, 1)
    }
    MouseArea{
        property real offsetX: 0
        property bool picked: false
        id: mouseArea
        drag.axis: Drag.XAxis
        drag.minimumX: -width*0.5
        drag.maximumX: parent.parent.width - width*0.5
        anchors.fill: parent
        onPressed: {
            activated(pointer)
            offsetX = mouse.x
        }

        onDoubleClicked: {
            if(picked) return
            var newObject = Qt.createQmlObject('import QtQuick 2.12; import QtQuick.Window 2.12;
                                                Window {width: 254; height: 240; visible: true; title: "ColorPicker"
                                                Rectangle{anchors.fill: parent; color: "#3A3C3E"}}',
                                               pointer,
                                               ":\qml\GradientPointer")
            var colorPickerComponent = Qt.createComponent("colorpicker/ColorPicker.qml")
            if(colorPickerComponent.status == Component.Ready) {
                var colorPickerObject = colorPickerComponent.createObject(newObject)
                picked = true
                colorPickerObject.width = Qt.binding(function() {return newObject.width})
                colorPickerObject.height = Qt.binding(function() {return newObject.height})
                colorPickerObject.startingColor = colorPointer.color
                colorPickerObject.colorValueChanged.connect(setColorValue)
                newObject.closing.connect(function(){mouseArea.picked = false})

                function setColorValue() {
                    colorPointer.color = colorPickerObject.colorValue
                }
            }
        }
        onPositionChanged: {
            var x = mouseArea.mapToItem(pointer.parent, mouse.x-offsetX, mouse.y).x
            x = Math.min(Math.max(x, -width*0.5), parent.parent.width - width*0.5)
            proportionX = (x + width*0.5)/pointer.parent.width
        }
    }
}
