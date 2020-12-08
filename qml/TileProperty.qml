import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startOffsetX: offsetXParam.propertyValue
    property alias startOffsetY: offsetYParam.propertyValue
    property alias startColumns: columnsParam.propertyValue
    property alias startRows: rowsParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startRotation: rotationParam.propertyValue
    property alias startRandPosition: randPositionParam.propertyValue
    property alias startRandRotation: randRotationParam.propertyValue
    property alias startRandScale: randScaleParam.propertyValue
    property alias startMask: maskStrengthParam.propertyValue
    property alias startInputsCount: inputsCountParam.propertyValue
    property alias startKeepProportion: keepProportionParam.checked
    signal offsetXChanged(real offset)
    signal offsetYChanged(real offset)
    signal columnsChanged(int columns)
    signal rowsChanged(int rows)    
    signal scaleXChanged(real scale)
    signal scaleYChanged(real scale)
    signal rotationAngleChanged(int angle)
    signal randPositionChanged(real rand)
    signal randRotationChanged(real rand)
    signal randScaleChanged(real rand)
    signal maskChanged(real mask)
    signal inputsCountChanged(int count)
    signal keepProportionChanged(bool keep)
    ParamSlider {
        id: columnsParam
        propertyName: "Columns"
        minimum: 1
        maximum: 20
        step: 1
        onPropertyValueChanged: {
            columnsChanged(columnsParam.propertyValue)
        }
    }
    ParamSlider {
        id: rowsParam
        y: 33
        propertyName: "Rows"
        minimum: 1
        maximum: 20
        step: 1
        onPropertyValueChanged: {
            rowsChanged(rowsParam.propertyValue)
        }
    }
    ParamSlider {
        id: offsetXParam
        y: 66
        propertyName: "Offset X"
        onPropertyValueChanged: {
            offsetXChanged(offsetXParam.propertyValue)
        }
    }
    ParamSlider {
        id: offsetYParam
        y: 99
        propertyName: "Offset Y"
        onPropertyValueChanged: {
            offsetYChanged(offsetYParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 132
        maximum: 4
        propertyName: "Scale X"
        onPropertyValueChanged: {
            scaleXChanged(scaleXParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 165
        maximum: 4
        propertyName: "Scale Y"
        onPropertyValueChanged: {
            scaleYChanged(scaleYParam.propertyValue)
        }
    }
    ParamSlider {
        id: rotationParam
        y: 198
        maximum: 360
        step: 1
        propertyName: "Rotation"
        onPropertyValueChanged: {
            rotationAngleChanged(rotationParam.propertyValue)
        }
    }
    ParamSlider {
        id: randPositionParam
        y: 231
        propertyName: "Randomizing position"
        onPropertyValueChanged: {
            randPositionChanged(randPositionParam.propertyValue)
        }
    }
    ParamSlider {
        id: randRotationParam
        y: 264
        propertyName: "Randomizing rotation"
        onPropertyValueChanged: {
            randRotationChanged(randRotationParam.propertyValue)
        }
    }
    ParamSlider {
        id: randScaleParam
        y: 297
        propertyName: "Randomizing scale"
        onPropertyValueChanged: {
            randScaleChanged(randScaleParam.propertyValue)
        }
    }
    ParamSlider {
        id: maskStrengthParam
        y: 330
        propertyName: "Mask"
        onPropertyValueChanged: {
            maskChanged(maskStrengthParam.propertyValue)
        }
    }
    ParamSlider {
        id: inputsCountParam
        y: 363
        minimum: 1
        maximum: 6
        step: 1
        propertyName: "Inputs count"
        onPropertyValueChanged: {
            inputsCountChanged(propertyValue)
        }
    }
    CheckBox {
        id: keepProportionParam
        y: 411
        leftPadding: 30
        height: 25
        width: 140
        text: qsTr("Keep proportion")
        checked: false
        onCheckedChanged: {
            keepProportionChanged(keepProportionParam.checked)
            focus = false
        }

        indicator: Item {
            implicitWidth: 30
            implicitHeight: 30
            x: keepProportionParam.contentItem.width + 5
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
                    visible: keepProportionParam.checked
                    color: "#A2A2A2"
                }
            }
        }

        contentItem: Text {
            topPadding: 0
            text: keepProportionParam.text
            color: "#A2A2A2"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            renderType: Text.NativeRendering
        }
    }
}
