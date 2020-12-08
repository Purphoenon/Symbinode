import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startInterpolation: control.currentIndex
    property alias startRadius: radiusParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    signal interpolationChanged(int interpolation)
    signal radiusChanged(real radius)
    signal smoothValueChanged(real smooth)

    ParamDropDown {
        id: control
        y: 15
        model: ["Linear", "Hermite"]
        onActivated: {
            if(index == 0) {
                interpolationChanged(0)
            }
            else if(index == 1) {
                interpolationChanged(1)
            }
            focus = false
        }
    }
    ParamSlider {
        id: radiusParam
        y: 38
        propertyName: "Radius"
        onPropertyValueChanged: {
            radiusChanged(radiusParam.propertyValue)
        }
    }
    ParamSlider {
        id: smoothParam
        y: 71
        propertyName: "Smooth"
        onPropertyValueChanged: {
            smoothValueChanged(smoothParam.propertyValue)
        }
    }
}
