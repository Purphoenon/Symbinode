import QtQuick 2.12
import QtQuick.Controls 2.5

CheckBox {
    id: param
    leftPadding: 14
    height: 26
    width: 125
    text: qsTr("Param name")
    checked: false

    indicator:
        Rectangle {
            x: parent.width - width - 5
            y: parent.height / 2 - height / 2
            width: 14
            height: 14
            radius: 6.5
            color: "#484C51"
            Rectangle {
                anchors.centerIn: parent
                width: 6
                height: 6
                radius: 3
                visible: param.checked
                color: "#B2B2B2"
            }
        }

    contentItem: Text {
        topPadding: 0
        text: param.text
        color: "#B2B2B2"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
