import QtQuick 2.12

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startSeed: seedParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal seedChanged(int seed)
    signal bitsChanged(int bitstType)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ParamDropDown {
        id: bitsParam
        y: 15
        model: ["8 bits", "16 bits"]
        onCurrentIndexChanged: {
            if(currentIndex == 0) {
                bitsChanged(0)
            }
            else if(currentIndex == 1) {
                bitsChanged(1)
            }
            focus = false
        }
        onActivated: {
            propertyChangingFinished("startBits", currentIndex, oldIndex)
        }
    }
    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 53
        clip: true
        ParamSlider {
            id: seedParam
            propertyName: "Seed"
            minimum: 1
            maximum: 100
            step: 1
            onPropertyValueChanged: {
                seedChanged(propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startSeed", propertyValue, oldValue)
            }
        }
    }
}
