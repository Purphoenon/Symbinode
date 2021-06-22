import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias mode: control.currentIndex
    property alias startIntensity: intensityParam.propertyValue
    property alias startSamples: samplesParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal blendModeChanged(int mode)
    signal intensityChanged(real intensity)
    signal samplesChanged(int samples)
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
    ParamDropDown{
        id: control
        y: 53
        model: ["Average", "Min", "Max"]
        onCurrentIndexChanged: {
            blendModeChanged(currentIndex)

            focus = false
        }

        onActivated: {
            propertyChangingFinished("mode", currentIndex, oldIndex)
        }
    }
    ParamSlider {
        id: intensityParam
        y: 76
        propertyName: "Intensity"
        onPropertyValueChanged: {
            intensityChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startIntensity", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: samplesParam
        y: 109
        propertyName: "Samples"
        maximum: 16
        minimum: 1
        step: 1
        onPropertyValueChanged: {
            samplesChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSamples", propertyValue, oldValue)
        }
    }
}
