/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.5

Item {
    property real minimumHeight: 200
    property string title: "Title"
    signal resized(bool resized)
    id: dragContainer
    width: parent.width
    height: parent.height
    Drag.active: mouseArea.drag.active
    Drag.hotSpot.x: width/2
    Drag.hotSpot.y: height/2
    Drag.keys: ["dragContainer"]

    Rectangle {
        anchors.fill: parent
        color: "#3A3C3E"        
        Rectangle {
            width: parent.width
            height: 25
            color: "#343537"
            Label {
                id: titleLabel
                width:parent.width
                height: 24
                text: dragContainer.title
                renderType: Text.NativeRendering
                leftPadding: 15
                topPadding: 0
                bottomPadding: 3
                font.pointSize: 10
                color: "#A2A2A2"
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
    MouseArea
    {
        id: mouseArea
        width: parent.width
        height: 25
        drag.target: dragContainer
        onPressed: {
            dragContainer.z = 1
            dragContainer.parent.parent.z = 2
        }

        onReleased: {
            console.log("released")
            if(!dragContainer.Drag.target) {
                dragContainer.x = 0
                dragContainer.parent.allotSpace()
            }
            dragContainer.Drag.drop()
            dragContainer.z = 0
            dragContainer.parent.parent.z = 1

        }
    }
    MouseArea {
        id: sizeChanger
        width: parent.width
        height: 4
        cursorShape: Qt.SizeVerCursor
        y: parent.height - 4
        z: 1
        function resize(container, offset) {
            var childrenItem = []
            for(var i = 0; i < container.parent.children.length; ++i) {
                var item = container.parent.children[i]
                var index = childrenItem.length
                for(var j = 0; j < childrenItem.length; ++j) {
                    if(childrenItem[j].y > item.y) {
                        index = j
                        break
                    }
                }
                childrenItem.splice(index, 0, item)
            }
            if(childrenItem.length > 1 && childrenItem.indexOf(container) == childrenItem.length - 1) return

            var targetIndex = childrenItem.indexOf(container)
            var newHeight = container.height + offset
            if(newHeight < container.minimumHeight) {
                offset = container.minimumHeight - container.height
                container.height = container.minimumHeight
                if(targetIndex + 1 < childrenItem.length) {
                    childrenItem[targetIndex + 1].height -= offset
                }
            }
            else {
                if(targetIndex + 1 < childrenItem.length) {
                    var item = childrenItem[targetIndex + 1]
                    if(item.height - offset < item.minimumHeight) {
                        offset = item.height - item.minimumHeight
                        item.height = item.minimumHeight
                    }
                    else {
                        item.height -= offset
                    }
                }
                else if(container.parent.height < container.parent.childrenRect.height + offset) {
                    offset = container.parent.height - container.parent.childrenRect.height
                }

                container.height += offset
            }

            var lastY = container.y + container.height
            var step = -(offset/(childrenItem.length - targetIndex - 1))
            for(var i = targetIndex + 1; i < childrenItem.length; ++i) {
                var item = childrenItem[i]
                item.y = lastY
                lastY += item.height
            }
        }

        onPositionChanged: {
            resized(true)
            dragContainer.parent.containerHeightChange(dragContainer, mouse.y)
        }

        onReleased: {
            resized(false)
        }

        Rectangle {
            anchors.bottom: parent.bottom
            height: 2
            width: parent.width
            color: "#2C2D2F"
        }
    }

    MouseArea {
        x: dragContainer.parent.parent.position === "right" ? 0 : dragContainer.width - width
        y: dragContainer.height - 8
        z: 1
        cursorShape: dragContainer.parent.parent.position === "right" ? Qt.SizeBDiagCursor : Qt.SizeFDiagCursor
        width: 6
        height: 6
        onPositionChanged: {
            resized(true)
            dragContainer.parent.containerSizeChange(dragContainer, mouse.x, mouse.y)
        }
        onReleased: {
            resized(false)
        }

        Canvas {
            id: resizeCanvas
            width: 6
            height: 6
            contextType: "2d"
            rotation: dragContainer.parent.parent.position === "right" ? 0 : -90
            onPaint: {
                var ctx = getContext("2d")
                ctx.reset();
                ctx.fillStyle = "#4DA2A2A2"
                ctx.lineTo(0, 0)
                ctx.lineTo(0, 6)
                ctx.lineTo(6, 6)
                ctx.fill()
            }
        }
    }
}
