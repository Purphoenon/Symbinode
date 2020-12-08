import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startIntensity: intensityParam.propertyValue
    signal intensityChanged(real intensity)
    ParamSlider {
        id: intensityParam
        maximum: 2
        propertyName: "Intensity"
        onPropertyValueChanged: {
            intensityChanged(intensityParam.propertyValue)
        }
    }
}
