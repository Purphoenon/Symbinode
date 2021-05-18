import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startColumns: columnsParam.propertyValue
    property alias startRows: rowsParam.propertyValue
    property alias startOffset: offsetParam.propertyValue
    property alias startWidth: widthParam.propertyValue
    property alias startHeight: heightParam.propertyValue
    property alias startMask: maskParam.propertyValue
    property alias startSmoothX: smoothXParam.propertyValue
    property alias startSmoothY: smoothYParam.propertyValue
    property alias startSeed: seedParam.propertyValue
    signal columnsChanged(int columns)
    signal rowsChanged(int rows)
    signal offsetChanged(real offset)
    signal bricksWidthChanged(real width)
    signal bricksHeightChanged(real height)
    signal maskChanged(real mask)
    signal smoothXChanged(real smooth)
    signal smoothYChanged(real smooth)
    signal seedChanged(int seed)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamSlider {
        id: columnsParam
        step: 1
        minimum: 1
        maximum: 20
        propertyName: "Columns"
        onPropertyValueChanged: {
            columnsChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startColumns", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: rowsParam
        y: 33
        minimum: 1
        maximum: 20
        step: 1
        propertyName: "Rows"
        onPropertyValueChanged: {
            rowsChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startRows", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: offsetParam
        y: 66
        propertyName: "Offset"
        onPropertyValueChanged: {
            offsetChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startOffset", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: widthParam
        y: 99
        propertyName: "Bricks Width"
        onPropertyValueChanged: {
            bricksWidthChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startWidth", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: heightParam
        y: 132
        propertyName: "Bricks Height"
        onPropertyValueChanged: {
            bricksHeightChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startHeight", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: smoothXParam
        y: 165
        propertyName: "Smooth X"
        onPropertyValueChanged: {
            smoothXChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSmoothX", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: smoothYParam
        y: 198
        propertyName: "Smooth Y"
        onPropertyValueChanged: {
            smoothYChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSmoothY", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: maskParam
        y: 231
        propertyName: "Mask"
        onPropertyValueChanged: {
            maskChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startMask", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: seedParam
        y: 264
        propertyName: "Seed"
        minimum: 1
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            seedChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSeed", propertyValue, oldValue)
        }
    }
}
