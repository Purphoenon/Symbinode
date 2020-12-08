import QtQuick 2.12

import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property real startMetal: 0
    signal metalChanged(real val)
    ParamSlider {
        id: metalParam
        propertyName: "Metalness"
        propertyValue: startMetal
        onPropertyValueChanged: {
            metalChanged(metalParam.propertyValue)
        }
    }
}
