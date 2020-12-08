import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: brightnessContrastProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startBrightness: brightnessParam.propertyValue
    property alias startContrast: contrastParam.propertyValue
    signal brightnessChanged(real value)
    signal contrastChanged(real value)
    ParamSlider {
        id: brightnessParam
        propertyName: "Brightness"
        minimum: -1
        onPropertyValueChanged: {
            brightnessChanged(brightnessParam.propertyValue)
        }
    }
    ParamSlider {
        id: contrastParam
        y: 33
        propertyName: "Contrast"
        minimum: -1
        onPropertyValueChanged: {
            contrastChanged(contrastParam.propertyValue)
        }
    }
}
