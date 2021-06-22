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
    property alias startBits: control.currentIndex
    signal columnsChanged(int columns)
    signal rowsChanged(int rows)
    signal offsetChanged(real offset)
    signal bricksWidthChanged(real width)
    signal bricksHeightChanged(real height)
    signal maskChanged(real mask)
    signal smoothXChanged(real smooth)
    signal smoothYChanged(real smooth)
    signal seedChanged(int seed)
    signal bitsChanged(int bitsType);
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: control
        y: 15
        model: ["8 bits per channel", "16 bits per channel"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                bitsChanged(0)
            }
            else if(currentIndex == 1) {
                bitsChanged(1)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startBits", currentIndex, oldIndex)
        }
    }
    ParamSlider {
        id: columnsParam
        y: 38
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
        y: 71
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
        y: 104
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
        y: 137
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
        y: 170
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
        y: 203
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
        y: 236
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
        y: 269
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
        y: 302
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
