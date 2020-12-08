import QtQuick 2.12
import QtQuick.Controls 2.5
import "colorpicker/"

Item {
    id: albedoProp
    height: parent.parent.height
    width: parent.width
    property vector3d startColor: Qt.vector3d(1.0, 1.0, 1.0)
    signal albedoChanged(vector3d albedo)
    ColorPicker {
        id: color
        height: parent.parent.height - 15
        width: parent.width
        startingColor: Qt.rgba(startColor.x, startColor.y, startColor.z, 1.0)
        onColorValueChanged: {
            albedoChanged(Qt.vector3d(colorValue.r, colorValue.g, colorValue.b))
        }
    }
}
