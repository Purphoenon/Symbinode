import QtQuick 2.12

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startRadius: radiusParam.propertyValue
    property alias startContrast: contrastParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal radiusChanged(real radius)
    signal contrastChanged(real contrast)
    signal bitsChanged(int bitstType)
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
            id: radiusParam
            propertyName: "Radius"
            maximum: 64
            onPropertyValueChanged: {
                radiusChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startRadius", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: contrastParam
            propertyName: "Contrast"
            y: 18
            minimum: -1
            onPropertyValueChanged: {
                contrastChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startContrast", propertyValue, oldValue)
            }
        }
    }
}
