import QtQuick 2.12
import QtQuick.Shapes 1.12

Shape {
    property int startX: 0
    property int startY: 0
    id: cut
    ShapePath {
        id: cutPath
        fillColor: "transparent"
        startX: cut.startX
        startY: cut.startY
        strokeWidth: 2
        strokeColor: "white"
        strokeStyle: ShapePath.DashLine
        dashPattern: [3, 3]
    }
    function addLine(x, y) {

        var pathLine = Qt.createQmlObject("import QtQuick 2.12; PathLine{}", cutPath, "CutLine")
        pathLine.x = x
        pathLine.y = y
        cutPath.pathElements.push(pathLine)
        console.log(pathLine.x)
    }
}
