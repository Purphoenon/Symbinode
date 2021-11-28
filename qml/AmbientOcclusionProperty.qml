import QtQuick 2.12

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startRadius: radiusParam.propertyValue
    property alias startSamples: samplesParam.propertyValue
    property alias startStrength: strengthParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal radiusChanged(real radius)
    signal samplesChanged(int samples)
    signal strengthChanged(real strength)
    signal ambientSmoothChanged(real smooth)
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
            onPropertyValueChanged: {
                radiusChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startRadius", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: samplesParam
            propertyName: "Samples"
            y: 18
            minimum: 4
            maximum: 16
            step: 1
            onPropertyValueChanged: {
                samplesChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startSamples", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: strengthParam
            propertyName: "Strength"
            y: 51
            onPropertyValueChanged: {
                strengthChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startStrength", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: smoothParam
            propertyName: "Smooth"
            y: 84
            onPropertyValueChanged: {
                ambientSmoothChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startSmooth", propertyValue, oldValue)
            }
        }
    }
}
