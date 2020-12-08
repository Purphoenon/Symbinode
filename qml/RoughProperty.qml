import QtQuick 2.12

import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property real startRough: 0.2
    signal roughChanged(real val)
    ParamSlider {
        id: roughParam
        propertyName: "Roughness"
        propertyValue: startRough
        onPropertyValueChanged: {
            roughChanged(roughParam.propertyValue)
        }
    }
}
