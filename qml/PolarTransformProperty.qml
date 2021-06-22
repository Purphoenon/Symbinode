import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startRadius: radiusParam.propertyValue
    property alias startClamp: clampParam.checked
    property alias startRotation: rotationParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal radiusChanged(real radius)
    signal clampChanged(bool clamp)
    signal angleChanged(int angle)
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
        id: radiusParam
        y: 38
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
        y: 71
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
        y: 119
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
