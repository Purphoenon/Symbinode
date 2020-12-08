import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: mappingProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startInputMin: inputMinParam.propertyValue
    property alias startInputMax: inputMaxParam.propertyValue
    property alias startOutputMin: outputMinParam.propertyValue
    property alias startOutputMax: outputMaxParam.propertyValue
    signal inputMinChanged(real value)
    signal inputMaxChanged(real value)
    signal outputMinChanged(real value)
    signal outputMaxChanged(real value)
    ParamSlider {
        id: inputMinParam
        propertyName: "Input Min"
        onPropertyValueChanged: {
            inputMinChanged(inputMinParam.propertyValue)
        }
    }
    ParamSlider {
        id: inputMaxParam
        y: 33
        propertyName: "Input Max"
        onPropertyValueChanged: {
            inputMaxChanged(inputMaxParam.propertyValue)
        }
    }
    ParamSlider {
        id: outputMinParam
        y: 66
        propertyName: "Output Min"
        onPropertyValueChanged: {
            outputMinChanged(outputMinParam.propertyValue)
        }
    }
    ParamSlider {
        id: outputMaxParam
        y: 99
        propertyName: "Output Max"
        onPropertyValueChanged: {
            outputMaxChanged(outputMaxParam.propertyValue)
        }
    }
}
