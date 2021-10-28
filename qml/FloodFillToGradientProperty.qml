import QtQuick 2.12

Item {
    width: parent.width
    height: childrenRect.height + 30
    property alias startRotation: rotationParam.propertyValue
    property alias startRandomizing: randomizingParam.propertyValue
    property alias startSeed: seedParam.propertyValue
    property alias startBits: bitsParam.currentIndex
    signal rotationAngleChanged(int rotation)
    signal randomizingChanged(real rand)
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
            id: rotationParam
            propertyName: "Rotation"
            maximum: 360
            step: 1
            onPropertyValueChanged: {
                rotationAngleChanged(rotationParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startRotation", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: randomizingParam
            y: 18
            propertyName: "Randomizing"
            onPropertyValueChanged: {
                randomizingChanged(randomizingParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startRandomizing", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: seedParam
            y: 51
            minimum: 1
            maximum: 100
            step: 1
            propertyName: "Seed"
            onPropertyValueChanged: {
                seedChanged(seedParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startSeed", propertyValue, oldValue)
            }
        }
    }
}
