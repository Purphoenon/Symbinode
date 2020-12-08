import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    property alias minimum: slider.from
    property alias maximum: slider.to
    property alias step: slider.stepSize
    property alias propertyName: label.text
    property alias propertyValue: slider.value

    width: parent.width
    height: childrenRect.height
    Label {
       id: label
       height: 25
       width: 100
       verticalAlignment: Text.AlignVCenter
       text: "Размер"
       color: "#D8D9D9"
       x: 30
       y: 15
       z: 1
    }

    Slider {
        id: slider
        x: 5
        y: 15
        width: parent.width - 10
        height: 25
        clip: true
        stepSize: 0.01
        onValueChanged: {
            valueBox.value = value.toFixed(2)
            focus = false
        }

        Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: slider.availableWidth
            height: 25
            clip: true
            z: 1
            color: "transparent"
            Canvas {
                id: mycanvas
                x: slider.visualPosition * (slider.availableWidth) - 2
                y: slider.topPadding + slider.availableHeight / 2 - parent.height / 2
                width: 4
                height: 3
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.fillStyle = Qt.rgba(0.84, 0.84, 0.84, 1);
                    ctx.lineTo(4, 0)
                    ctx.lineTo(2, 3)
                    ctx.lineTo(0, 0)
                    ctx.fill()
                }
            }
        }

        handle: Item{
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: 4
            height: 25
            Rectangle {
                visible: (parent.x > 3 + slider.leftPadding) && (parent.x < slider.availableWidth - 3 +
                                                                 slider.leftPadding)
                x: -3
                width: 3
                height: 25
                color: "#57758F"
            }
        }

        background:
            Rectangle {
                    x: slider.leftPadding
                    y: slider.topPadding + slider.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 25
                    width: slider.availableWidth
                    height: implicitHeight
                    color: "#484C51"
                    radius: 3

                    Rectangle {
                        width: slider.visualPosition * parent.width
                        height: parent.height
                        color: "#57758F"
                        radius: 3
                    }
                }
        from: 0
        to: 1
    }

    StepValueBox {
        id: valueBox
        x: parent.width - 25
        y: 15
        width: 20
        height: 25
        from: slider.from
        to: slider.to
        step: slider.stepSize
        value: slider.value
        onNewValueChanged: {
            slider.value = value
        }
    }
}
