import QtQuick 2.0
import QtQuick.Controls 2.5

Rectangle {
    ColorSettings {
        id:colors
    }
    id: socket
    property bool showTip: false
    property string textTip: "Socket"
    property real scaleView: 1.0
    property int type: 0
    property bool mask: false
    property bool additional: false
    y: 0
    x: 0
    width: parent.width
    height: parent.height
    radius: 8*scaleView
    color: mask ? "#AFAFAF" : "#FEC556"
    border.width: 3*scaleView
    border.color: "#2D2D2D"

    Rectangle {
        x: socket.width/2
        z: -1
        visible: additional
        height: socket.height
        width: 180*scaleView
        color: "#2D2D2D"//colors.node_title_background
    }

    Text {
        id: tip
        text: textTip
        visible: showTip && socket.parent.parent.parent
        y: (socket.height - height)*0.5
        x: type ? socket.width + 5 : -width - 5
        color: "#D8D9D9"
    }
    /*ToolTip {
        id: tip
        text: textTip
        visible: showTip && socket.parent.parent.parent
        y: (socket.height - height)*0.5
        x: type ? socket.width : -width
        contentItem: Text {
                text: tip.text
                font: tip.font
                color: "#D8D9D9"
        }

        background: Item{}
    }*/
}
