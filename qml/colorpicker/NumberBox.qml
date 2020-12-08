import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    property alias name: label.text
    property alias numberBox: textBox
    property real maximumValue: 360
    property real offsetX: 14
    property real boxWidth: 25
    Label {
        id: label
        text: "H"
        y: 4
        color: "#E0E0E0"
        font.pointSize: 8
        font.bold: true
    }

    Rectangle {
        width: boxWidth + 5
        height: 17
        radius: 3
        x: offsetX - 5
        y: 2
        border.width: 1
        border.color: "#6B737B"
        color: "transparent"
    }

    TextInput {
        id: textBox
        width: boxWidth
        height: 20
        x: offsetX
        color: "#E0E0E0"
        font.pointSize: 8
        selectByMouse: true
        validator: IntValidator{bottom: 0; top: maximumValue;}
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        onAccepted: {
            focus = false
        }
    }
}
