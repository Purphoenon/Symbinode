import QtQuick 2.12

Item {
    id: colorSlider
    property real value: (1 - pickerCursor.x/width)
    property real proportionX: 0
    signal hueChanged(real hueValue)
    width: 300; height: 15
    Item {
        id: pickerCursor
        height: parent.height
        x: proportionX * parent.width
        Rectangle {
            x: -7; y: 0
            width: 7*2; height: 7*2
            radius: 7
            border.color: "black"; border.width: 1
            color: "transparent"
            Rectangle {
                anchors.fill: parent; anchors.margins: 1;
                border.color: "white"; border.width: 1
                radius: width/2
                color: "transparent"
            }
        }
    }
    MouseArea {
        x: -Math.round(7/2)
        width: parent.width + 7
        height: 14
        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                proportionX = Math.max(0, Math.min(1, mouse.x/parent.width))
                value = (1 - pickerCursor.x/parent.width)
            }
        }
        onPositionChanged: {
            handleMouse(mouse)
            hueChanged(colorSlider.value)
        }
        onPressed: handleMouse(mouse)
    }
}
