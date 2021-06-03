import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startRadius: radiusParam.propertyValue
    property alias startClamp: clampParam.checked
    property alias startRotation: rotationParam.propertyValue
    signal radiusChanged(real radius)
    signal clampChanged(bool clamp)
    signal angleChanged(int angle)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamSlider {
        id: radiusParam
        maximum: 10
        propertyName: "Radius"
        onPropertyValueChanged: {
            radiusChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startRadius", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: rotationParam
        y: 33
        maximum: 360
        step: 1
        propertyName: "Rotation"
        onPropertyValueChanged: {
            angleChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startRotation", propertyValue, oldValue)
        }
    }
    ParamCheckbox {
        id: clampParam
        y: 81
        width: 75
        text: qsTr("Clamp")
        checked: false
        onCheckedChanged: {
            clampChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startClamp", checked, !checked)
            focus = false
        }
    }
}
