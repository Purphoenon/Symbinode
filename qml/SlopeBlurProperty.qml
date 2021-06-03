import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias mode: control.currentIndex
    property alias startIntensity: intensityParam.propertyValue
    property alias startSamples: samplesParam.propertyValue
    signal blendModeChanged(int mode)
    signal intensityChanged(real intensity)
    signal samplesChanged(int samples)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown{
        id: control
        y: 15
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
        y: 38
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
        y: 71
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
