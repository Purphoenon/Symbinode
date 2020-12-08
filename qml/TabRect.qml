import QtQuick 2.11
import QtQuick.Controls 2.5

Rectangle {
    id: tab
    property bool selected: true
    property string fileName: "New"
    property Item baseParent
    signal activated()
    signal closed()
    Drag.active: mouseArea.drag.active
    Drag.hotSpot.x: width/2
    Drag.hotSpot.y: height/2
    Drag.keys: ["tabsPanel"]
    width: parent.width
    height: parent.height - 1
    color: selected ? "#3A3C3E" : "#363839"
    states: State {
        when: mouseArea.drag.active
        ParentChange { target: tab; parent: tabsArea }
        AnchorChanges { target: tab; anchors.verticalCenter: undefined }
    }
    MouseArea {
        id: mouseArea
        width: parent.width
        height: parent.height
        drag.target: parent
        drag.axis: Drag.XAxis
        drag.minimumX: 0
        drag.maximumX: tab.parent.parent.parent.width - width
        onPressed: {
            if(!tab.selected) activated()
            tab.baseParent = tab.parent
            tab.width = tab.parent.width
            tab.parent = tab.parent.parent.parent
            tab.x = tab.baseParent.x
            tab.y = tab.baseParent.y
        }
        onReleased: {
            tab.parent = tab.baseParent            
            tab.width = Qt.binding(function(){return tab.parent.width})
            tab.Drag.drop()
            tab.x = 0
        }
    }

    Label {
        id: titleLabel
        width: parent.width - 20
        height: parent.height
        text: fileName
        renderType: Text.NativeRendering
        leftPadding: 15
        clip: true
        topPadding: 0
        bottomPadding: 3
        font.pointSize: 10
        color: "#A2A2A2"
        verticalAlignment: Text.AlignVCenter
    }
    Image {
        y: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        source: "qrc:/icons/tab-close.svg"
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                closed()
            }
        }
    }
  }
