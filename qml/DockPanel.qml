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

import QtQuick 2.0

Rectangle {
    property string position: "left"
    property real offset: 255
    property real limit: 255
    property real dockX: position === "right" ? parent.width - offset : offset
    property alias container: flowContainer
    signal resized(bool resized)
    id: dock
    x: position === "right" ? dockX : 0
    width: position === "right" ? parent.width - x : dockX
    height: parent.height
    color: flowContainer.contain ? "#3A3C3E" : "transparent"
    MouseArea {
        anchors.fill: parent
        enabled: flowContainer.contain
        acceptedButtons: Qt.AllButtons
        onClicked: {
            focus = true
            focus = false
        }
        onWheel: {

        }
    }

    DropArea {
        id: dropArea
        property string key: "dragContainer"
        property var childrenList: flowContainer.children
        keys: [key]
        anchors.fill: parent

        function order(draged, dropped) {
            if(dropped) console.log("order")

            var childrenItem = []
            if(draged) childrenItem.push(drag.source)
            for(var i = 0; i < flowContainer.children.length; ++i) {
                var item = flowContainer.children[i]
                if(item == drag.source) continue
                var index = childrenItem.length
                for(var j = 0; j < childrenItem.length; ++j) {
                    if(childrenItem[j].y > item.y) {
                        index = j
                        break
                    }
                }
                childrenItem.splice(index, 0, item)
            }

            var childrenHeight = 0
            for(var i = 0; i < childrenItem.length; ++i) {
                childrenHeight += childrenItem[i].height
            }
            if(flowContainer.height < childrenHeight) {
                var h = flowContainer.height/childrenItem.length
                for(var i = 0; i < childrenItem.length; ++i) {
                    childrenItem[i].height = h
                }
            }

            var lastY = 0
            for(var i = 0; i < childrenItem.length; ++i) {
                if(childrenItem[i] == drag.source && !dropped) {
                    lastY += childrenItem[i].height
                    continue
                }
                childrenItem[i].y = lastY
                lastY += childrenItem[i].height
            }
        }

        onPositionChanged: {
            order(true, false)
        }

        onEntered: {
            gradient.visible = flowContainer.contain ? false : true
        }
        onExited: {
            console.log("exited")
            gradient.visible = false
            order(false, false)
        }
        onDropped: {
            var oldParent = drag.source.parent
            drag.source.parent = flowContainer
            order(true, true)
            if(oldParent !== flowContainer) {
                if(oldParent.children.length > 1) oldParent.allotSpace()
                if(flowContainer.children.length > 1) flowContainer.allotSpace()
            }
            drag.source.x = 0
            gradient.visible = false
        }
    }

    Item {
        property bool contain: children.length > 0
        width: parent.width
        height: parent.height
        id: flowContainer
        function allotSpace() {
            var childrenItem = []
            for(var i = 0; i < flowContainer.children.length; ++i) {
                var item = flowContainer.children[i]
                var index = childrenItem.length
                for(var j = 0; j < childrenItem.length; ++j) {
                    if(childrenItem[j].y > item.y) {
                        index = j
                        break
                    }
                }
                childrenItem.splice(index, 0, item)
            }
            var h = flowContainer.height/flowContainer.children.length
            var lastY = 0
            for(var j = 0; j < childrenItem.length; ++j) {
                var item = childrenItem[j]
                item.height = h
                item.y = lastY
                lastY +=h
            }
        }
        function containerSizeChange(container, offsetX, offsetY) {
            dock.offset += position === "right" ? -offsetX : offsetX
            dock.offset = Math.min(Math.max(dock.offset, 255), dock.parent.width - limit)
            containerHeightChange(container, offsetY)
        }
        function containerHeightChange(container, offset) {
            var childrenItem = []
            for(var i = 0; i < flowContainer.children.length; ++i) {
                var item = flowContainer.children[i]
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
                else if(flowContainer.height < flowContainer.childrenRect.height + offset) {
                    offset = flowContainer.height - flowContainer.childrenRect.height
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
    }

    MouseArea {
        id: sizeChangeArea
        x: position === "right" ? -5 : dock.width
        z: -1
        width: 5
        height: parent.height
        cursorShape: Qt.SizeHorCursor
        visible: flowContainer.contain
        onPositionChanged: {
            resized(true)
            dock.offset += position === "right" ? -mouse.x : mouse.x
            dock.offset = Math.min(Math.max(dock.offset, 255), dock.parent.width - limit)
        }
        onReleased: {
            resized(false)
        }

        Rectangle {
            id: sizeChangeRect
            color: "#2C2D2F"
            width: 5
            height: parent.height
        }
    }

    Rectangle {
        id: gradient
        width: parent.height
        height: 40
        x: position === "right" ? parent.width : 0
        y:  position === "right" ? 0 : width
        visible: false
        transform: Rotation { origin.x: 0; origin.y: 0; angle: position === "right" ? 90 : 270}
        gradient: Gradient {
                GradientStop { position: 0.0; color: "#3b3b3d" }
                GradientStop { position: 1.0; color: "transparent" }
            }
    }
}
