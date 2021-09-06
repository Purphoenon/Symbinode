import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startIntensity: intensityParam.propertyValue
    property alias startAngle: angleParam.propertyValue
    property alias startBits: control.currentIndex
    signal intensityChanged(real intensity)
    signal angleChanged(int angle)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: control
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
            onPropertyValueChanged: {
                intensityChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startIntensity", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: angleParam
            y: 18
            propertyName: "Angle"
            maximum: 360
            step: 1
            onPropertyValueChanged: {
                angleChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startAngle", propertyValue, oldValue)
            }
        }
    }
}
