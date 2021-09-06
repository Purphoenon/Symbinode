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
        model: ["8 bits", "16 bits"]
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
    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 53
        clip: true
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
            y: 18
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
            y: 51
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
            y: 84
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
            y: 117
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
            y: 150
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
}
