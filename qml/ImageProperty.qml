import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

Item {
    width: parent.width
    height: childrenRect.height + 30
    property string fileName: "Open"
    property alias path: filePath.path
    property alias startTransX: transXParam.propertyValue
    property alias startTransY: transYParam.propertyValue
    signal openFile()
    signal loadTexture(string file)
    signal transXChanged(real x)
    signal transYChanged(real y)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    signal fileChosed(string path)
    onFileChosed: {
        filePath.path = path
        propertyChangingFinished("path", filePath.path, filePath.oldPath)
    }

    Rectangle {
        property string path
        property string oldPath
        id: filePath
        width: parent.width - 20
        height: 25
        x: 10
        y: 15
        color: "#484C51"
        border.width: 1
        border.color: "#2C2D2F"
        radius: 3
        onPathChanged: {
            loadTexture(path)
        }

        Label {
            id: titleLabel
            width: parent.width - 25
            height: parent.height
            text: fileName
            leftPadding: 12
            elide: Text.ElideRight
            topPadding: 0
            bottomPadding: 0
            color: "#B2B2B2"
            verticalAlignment: Text.AlignVCenter
        }
        Button {
            id: open
            x: parent.width - width - 1
            y: 1
            width: 23
            height: 23

            onClicked: {
                filePath.oldPath = filePath.path
                openFile()
            }
            highlighted: true

            background: Rectangle {
                    implicitWidth: 23
                    implicitHeight: 23
                    color: open.down ? "#A2A2A2" : "#4DA2A2A2";
                    //opacity: enabled ? 1 : 0.3
                    radius: 2
                }
        }
    }

    Item {
        width: parent.width - 40
        height: childrenRect.height
        x: 10
        y: 53
        clip: true
        ParamSlider {
            id: transXParam
            propertyName: "Translate X"
            minimum: -1
            onPropertyValueChanged: {
                transXChanged(transXParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startTransX", propertyValue, oldValue)
            }
        }
        ParamSlider {
            id: transYParam
            y: 18
            propertyName: "Translate Y"
            minimum: -1
            onPropertyValueChanged: {
                transYChanged(transYParam.propertyValue)
            }
            onChangingFinished: {
                propertyChangingFinished("startTransY", propertyValue, oldValue)
            }
        }
    }
}