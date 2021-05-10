import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startDistance: distanceParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    property alias startUseAlpha: useAlphaParam.checked
    signal distanceChanged(real dist)
    signal bevelSmoothChanged(real smooth)
    signal useAlphaChanged(bool use)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamSlider {
        id: distanceParam
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
        y: 33
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
        y: 81
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
