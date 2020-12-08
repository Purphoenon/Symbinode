import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: thresholdProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startThreshold: thresholdParam.propertyValue
    signal thresholdChanged(real value)
    ParamSlider {
        id: thresholdParam
        propertyName: "Threshold"
        onPropertyValueChanged: {
            thresholdChanged(thresholdParam.propertyValue)
        }
    }
}
