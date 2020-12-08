import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    width: parent.width
    height: childrenRect.height + 45
    property alias startNoiseScale: scaleParam.propertyValue
    property alias startScaleX: scaleXParam.propertyValue
    property alias startScaleY: scaleYParam.propertyValue
    property alias startLayers: layersParam.propertyValue
    property alias startPersistence: persistenceParam.propertyValue
    property alias startAmplitude: amplitudeParam.propertyValue
    property alias type: control.currentIndex
    signal noiseTypeChanged(string type)
    signal noiseScaleChanged(real scale)
    signal scaleXChanged(real scale)
    signal scaleYChanged(real scale)
    signal layersChanged(int layers)
    signal persistenceChanged(real persistence)
    signal amplitudeChanged(real aplitude)

    ParamDropDown {
        id: control
        y: 15
        onActivated: {
            if(index == 0) {
                noiseTypeChanged("noisePerlin")
            }
            else if(index == 1) {
                noiseTypeChanged("noiseSimple")
            }
            focus = false
        }
    }
    ParamSlider {
        id: scaleParam
        y: 38
        propertyName: "Scale"
        maximum: 100
        step: 1
        onPropertyValueChanged: {
            noiseScaleChanged(scaleParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleXParam
        y: 71
        propertyName: "Scale X"
        step: 1
        minimum: 1
        maximum: 20
        onPropertyValueChanged: {
            scaleXChanged(scaleXParam.propertyValue)
        }
    }
    ParamSlider {
        id: scaleYParam
        y: 104
        propertyName: "Scale Y"
        step: 1
        minimum: 1
        maximum: 20
        onPropertyValueChanged: {
            scaleYChanged(scaleYParam.propertyValue)
        }
    }
    ParamSlider {
        id: layersParam
        y: 137
        propertyName: "Layers"
        maximum: 16
        step: 1
        onPropertyValueChanged: {
            layersChanged(layersParam.propertyValue)
        }
    }
    ParamSlider {
        id: persistenceParam
        y: 170
        propertyName: "Persistence"
        onPropertyValueChanged: {
            persistenceChanged(persistenceParam.propertyValue)
        }
    }
    ParamSlider {
        id: amplitudeParam
        y: 203
        propertyName: "Amplitude"
        onPropertyValueChanged: {
            amplitudeChanged(amplitudeParam.propertyValue)
        }
    }
}
