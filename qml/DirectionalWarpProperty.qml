import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startIntensity: intensityParam.propertyValue
    property alias startAngle: angleParam.propertyValue
    signal intensityChanged(real intensity)
    signal angleChanged(int angle)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
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
        y: 33
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
