import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: mirrorProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startDirection: control.currentIndex
    signal directionChanged(int dir)

    ParamDropDown {
        id: control
        y: 15
        model: ["Left to right", "Right to left", "Top to bottom", "Bottom to top"]
        onActivated: {
            if(index == 0) {
                directionChanged(0)
            }
            else if(index == 1) {
                directionChanged(1)
            }
            else if(index == 2) {
                directionChanged(2)
            }
            else if(index == 3) {
                directionChanged(3)
            }
            focus = false
        }
    }
}
