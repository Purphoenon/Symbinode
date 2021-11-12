import QtQuick 2.12

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startIntensity: intensityParam.propertyValue
    property alias startOffset: offsetParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal intensityChanged(real intensity)
    signal offsetChanged(int offset)
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
            id: intensityParam
            propertyName: "Intensity"
            maximum: 10
            onPropertyValueChanged: {
                intensityChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startIntensity", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: offsetParam
            propertyName: "Offset"
            y: 18
            minimum: 1
            maximum: 10
            step: 1
            onPropertyValueChanged: {
                offsetChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startOffset", propertyValue, oldValue)
            }
        }
    }
}
