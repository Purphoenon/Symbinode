import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: bombingProp
    height: 115
    width: parent.width
    property alias startSize: sizeParam.propertyValue
    property alias startImagePerCell: imagePerCellParam.propertyValue
    signal cellSizeChanged(int size)
    signal imagePerCellChanged(int count)
    ParamSlider {
        id: sizeParam
        propertyName: "Size"
        minimum: 1
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            cellSizeChanged(sizeParam.propertyValue)
        }
    }
    ParamSlider {
        id: imagePerCellParam
        y: 33
        propertyName: "Image per cell"
        minimum: 1
        maximum: 3
        step: 1
        onPropertyValueChanged: {
            imagePerCellChanged(imagePerCellParam.propertyValue)
        }
    }
}
