import QtQuick 2.12
import QtQuick.Controls 2.5

ComboBox {
    property color popupColor: "#2C2D2F"
    id: control
    model: ["Perlin", "Simple"]
    leftPadding: 10
    rightPadding: 10
    width: parent.width - 10
    height: childrenRect.height

    delegate: ItemDelegate {
        width: control.width
        height: 25
        contentItem: Text {
            text: modelData
            color: "#A2A2A2"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex === index
        background: Rectangle {
            color: parent.highlighted ? "#404347" : "transparent"
        }
    }

    indicator: Canvas {
            id: canvas
            x: control.width - width - control.rightPadding
            y: control.topPadding + (control.availableHeight - height) / 2
            width: 6
            height: 4
            contextType: "2d"

            Connections {
                target: control
                function onPressedChanged() { canvas.requestPaint(); }
            }

            onPaint: {
                var context = getContext("2d");
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = "#A2A2A2"
                context.fill();
            }
        }

    contentItem: Text {
        leftPadding: 12
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        color: "#A2A2A2"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        x: parent.leftPadding
        y: parent.topPadding
        width: parent.width - parent.rightPadding
        height: 25
        color: "#484C51"
        border.width: 1
        border.color: "#2C2D2F"
        radius: 3
    }

    popup: Popup {
        x: parent.leftPadding
        y: control.height - 2 + control.topPadding
        width: control.width - parent.rightPadding
        implicitHeight: contentItem.implicitHeight
        padding: 0
        topPadding: 2

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: popupColor
            radius: 2
        }
    }
}
