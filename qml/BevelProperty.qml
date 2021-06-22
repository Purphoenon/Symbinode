import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startDistance: distanceParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startUseAlpha: useAlphaParam.checked
    property alias startBits: control.currentIndex
    signal distanceChanged(real dist)
    signal bevelSmoothChanged(real smooth)
    signal useAlphaChanged(bool use)
    signal bitsChanged(int bitsType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: control
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
        id: distanceParam
        y: 38
        minimum: -1
        propertyName: "Distance"
        onPropertyValueChanged: {
            distanceChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startDistance", propertyValue, oldValue)
        }
    }
    ParamSlider {
        id: smoothParam
        y: 71
        propertyName: "Smooth"
        onPropertyValueChanged: {
            bevelSmoothChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startSmooth", propertyValue, oldValue)
        }
    }
    ParamCheckbox {
        id: useAlphaParam
        y: 119
        width: 90
        text: qsTr("Use alpha")
        checked: false
        onCheckedChanged: {
            useAlphaChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startUseAlpha", checked, !checked)
            focus = false
        }
    }
}
