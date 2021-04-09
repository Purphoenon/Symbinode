import QtQuick 2.12

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startStartX: startXParam.propertyValue
    property alias startStartY: startYParam.propertyValue
    property alias startEndX: endXParam.propertyValue
    property alias startEndY: endYParam.propertyValue
    property alias startCenterWidth: whiteWidthParam.propertyValue
    property alias startTiling: tilingParam.checked
    property alias type: control.currentIndex
    signal gradientTypeChanged(string type)
    signal startXChanged(real x)
    signal startYChanged(real y)
    signal endXChanged(real x)
    signal endYChanged(real y)
    signal centerWidthChanged(real width)
    signal tilingChanged(bool tiling)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown{
        id: control
        y: 15
        model: ["Linear", "Reflected", "Angular", "Radial"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                gradientTypeChanged("linear")
            }
            else if(currentIndex == 1) {
                gradientTypeChanged("reflected")
            }
            else if(currentIndex == 2) {
                gradientTypeChanged("angular")
            }
            else if(currentIndex == 3) {
                gradientTypeChanged("radial")
            }
            focus = false
        }

        onActivated: {
            propertyChangingFinished("type", currentIndex, oldIndex)
        }
    }

    ParamSlider {
        id: startXParam
        y: 38
        propertyName: "Start X"
        onPropertyValueChanged: {
            startXChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startStartX", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: startYParam
        y: 71
        propertyName: "Start Y"
        onPropertyValueChanged: {
            startYChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startStartY", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: endXParam
        y: 104
        propertyName: "End X"
        onPropertyValueChanged: {
            endXChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startEndX", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: endYParam
        y: 137
        propertyName: "End Y"
        onPropertyValueChanged: {
            endYChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startEndY", propertyValue, oldValue)
        }
    }

    ParamSlider {
        id: whiteWidthParam
        visible: control.currentIndex == 1
        y: 170
        propertyName: "Center width"
        onPropertyValueChanged: {
            centerWidthChanged(propertyValue)
        }
        onChangingFinished: {
            propertyChangingFinished("startCenterWidth", propertyValue, oldValue)
        }
    }

    ParamCheckbox{
        id: tilingParam
        visible: control.currentIndex == 0 || control.currentIndex == 1
        y: control.currentIndex == 1 ? 218 : 185
        width: 70
        text: qsTr("Tiling")
        onCheckedChanged: {
            tilingChanged(checked)
        }
        onToggled: {
            propertyChangingFinished("startTiling", checked, !checked)
            focus = false
        }
    }
}
