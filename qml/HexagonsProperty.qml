import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startColumns: columnsParam.propertyValue
    property alias startRows: rowsParam.propertyValue
    property alias startSize: sizeParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startMask: maskParam.propertyValue
    property alias startSeed: seedParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal columnsChanged(int columns)
    signal rowsChanged(int rows)
    signal hexSizeChanged(real size)
    signal hexSmoothChanged(real smooth)
    signal maskChanged(real mask)
    signal seedChanged(int seed);
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: bitsParam
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
        id: sizeParam
        y: 104
        propertyName: "Size"
        onPropertyValueChanged: {
            hexSizeChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSize", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: smoothParam
        y: 137
        propertyName: "Smooth"
        onPropertyValueChanged: {
            hexSmoothChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSmooth", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: maskParam
        y: 170
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
        y: 203
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
