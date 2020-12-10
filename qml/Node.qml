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

import QtQuick 2.12
import QtQuick.Controls 2.5

DragRect {
    ColorSettings {
        id:colors
    }
    property real scaleView: 1.0
    property string title: "Title"
    property string contentLabel: ""
    id: node
    width: parent.width - 16*scaleView
    border.width: 0
    x: 8*scaleView
    y: 0
    color: "transparent"

    Rectangle {
        id: borders
        width: parent.width + 2*Math.max(1*scaleView, 1)
        height: parent.height + 2*Math.max(1*scaleView, 1)
        x: -Math.max(1*scaleView, 1)
        y: -Math.max(1*scaleView, 1)
        radius: 10*scaleView
        color: "transparent"
        border.width: Math.max(2*scaleView, 1)
        border.color: node.hovered ? colors.node_hovered : node.selected ? colors.node_selected : "transparent"
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 10*scaleView
            color: "transparent"
            border.width: Math.max(2*scaleView, 1)
            border.color: node.hovered ? colors.node_hovered : node.selected ? colors.node_selected : "transparent"
        }
    }

    Rectangle {
        id:content
        width: parent.width
        height: parent.height
        color: "#2D2D2D"//colors.node_content_background
        radius: 9*scaleView

        Rectangle {
                id: bottomRect
                z: -1
                anchors.bottom: parent.bottom
                width: parent.width
                height: 9*scaleView
                color: "#2D2D2D"//colors.node_title_background
        }
        Rectangle {
            x: 3*scaleView
            y: 3*scaleView
            width: parent.width - 6*scaleView
            height: 30*scaleView
            color: colors.node_title_background
            radius: 6*scaleView
            Label {
                id: titleLabel
                anchors.fill: parent
                text: node.title
                anchors.bottomMargin: 5*scaleView
                renderType: Text.NativeRendering
                font.pointSize: 10*scaleView
                color: "#C8C8C8"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: TextInput.AlignHCenter
            }
        }

        Rectangle {
            x: 3*scaleView
            y: 28*scaleView
            width: parent.width - 6*scaleView
            height: 2*scaleView
            color: "#3B3B3B"
        }
    }
    Rectangle {
        x: 3*scaleView
        y: 30*scaleView
        width: parent.width - 6*scaleView
        height: parent.height - 33*scaleView
        color: colors.node_title_background
    }
}
