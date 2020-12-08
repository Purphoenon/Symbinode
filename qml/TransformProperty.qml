import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startTransX: transXParam.propertyValue
    property alias startTransY: transYParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startRotation: rotationParam.propertyValue
    property alias startClamp: clampParam.checked
    signal transXChanged(real x)
    signal transYChanged(real y)
    signal scaleXChanged(real x)
    signal scaleYChanged(real y)
    signal angleChanged(int angle)
    signal clampCoordsChanged(bool clamp)
    ParamSlider {
        id: transXParam
        propertyName: "Translate X"
        minimum: -1
        onPropertyValueChanged: {
            transXChanged(transXParam.propertyValue)
        }
    }
    ParamSlider {
        id: transYParam
        y: 33
        propertyName: "Translate Y"
        minimum: -1
        onPropertyValueChanged: {
            transYChanged(transYParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 66
        propertyName: "Scale X"
        maximum: 2
        onPropertyValueChanged: {
            scaleXChanged(scaleXParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 99
        propertyName: "Scale Y"
        maximum: 2
        onPropertyValueChanged: {
            scaleYChanged(scaleYParam.propertyValue)
        }
    }
    ParamSlider {
        id: rotationParam
        y: 132
        propertyName: "Rotation"
        maximum: 360
        step: 1
        onPropertyValueChanged: {
            angleChanged(rotationParam.propertyValue)
        }
    }
    CheckBox {
        id: clampParam
        y: 180
        leftPadding: 30
        height: 25
        width: 100
        text: qsTr("Clamp")
        checked: false
        onCheckedChanged: {
            clampCoordsChanged(clampParam.checked)
        }

        indicator: Item {
                       implicitWidth: 30
                       implicitHeight: 30
                       x: clampParam.contentItem.width + 5
                       anchors.verticalCenter: parent.verticalCenter
                       Rectangle {
                           width: 14
                           height: 14
                           anchors.centerIn: parent
                           color: "transparent"
                           border.color: "#A2A2A2"
                           Rectangle {
                               width: 6
                               height: 6
                               anchors.centerIn: parent
                               visible: clampParam.checked
                               color: "#A2A2A2"
                           }
                       }
                   }

        contentItem: Text {
            topPadding: 0
            text: clampParam.text
            color: "#A2A2A2"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            renderType: Text.NativeRendering
        }
    }
}
