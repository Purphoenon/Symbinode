import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    height: childrenRect.height + 30
    width: parent.width
    property alias startMode: control.currentIndex
    property real startFactor
    signal modeChanged(int mode)
    signal factorChanged(real f)

    ParamDropDown {
        id: control
        y: 15
        model: ["Mix", "Add", "Multiply", "Substract", "Divide"]
        onActivated: {
            if(index == 0) {
                modeChanged(0)
            }
            else if(index == 1) {
                modeChanged(1)
            }
            else if(index == 2) {
                modeChanged(2)
            }
            else if(index == 3) {
                modeChanged(3)
            }
            else if(index == 4) {
                modeChanged(4)
            }
            focus = false
        }
    }

    ParamSlider {
        id: factorParam
        y: 38
        propertyName: "Factor"
        propertyValue: startFactor
        onPropertyValueChanged: {
            factorChanged(factorParam.propertyValue)
        }
    }
}
