import QtQuick 2.12

import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property real startStrenght: 0.2
    signal strenghtChanged(real val)
    ParamSlider {
        id: strenghtParam
        minimum: -1
        propertyName: "Strenght"
        propertyValue: startStrenght
        onPropertyValueChanged: {
            strenghtChanged(strenghtParam.propertyValue*10)
        }
    }
}
