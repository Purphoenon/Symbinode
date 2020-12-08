import QtQuick 2.0

Rectangle {
    id: node
    property real baseX: 40
    property real baseY: 50
    property real panX: 0
    property real panY: 0
    property bool selected: false
    property bool hovered: false
    width: 80; height: 100
    transformOrigin: Item.TopLeft
    border.color: node.hovered ? colors.node_hovered : node.selected ? colors.node_selected : "black"
    border.width: 1
    radius: node.radius
    color: node.color
 }


