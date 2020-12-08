import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startSides: sidesParam.propertyValue
    property alias startScale: scaleParam.propertyValue
    property alias startSmooth: smoothParam.propertyValue
    signal sidesChanged(int sides)
    signal polygonScaleChanged(real scale)
    signal polygonSmoothChanged(real smooth)
    ParamSlider {
        id: sidesParam
        propertyName: "Sides"
        maximum: 36
        minimum: 3
        step: 1
        onPropertyValueChanged: {
            sidesChanged(sidesParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleParam
        y: 33
        propertyName: "Scale"
        onPropertyValueChanged: {
            polygonScaleChanged(scaleParam.propertyValue)
        }
    }
    ParamSlider {
        id: smoothParam
        y: 66
        propertyName: "Smooth"
        onPropertyValueChanged: {
            polygonSmoothChanged(smoothParam.propertyValue)
        }
    }
}
