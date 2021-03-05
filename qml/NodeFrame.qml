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

Rectangle {
    property real scaleView: 1.0
    property bool selected: false
    property bool hovered: false
    property bool bubbleVisible: false
    property vector3d startColor
    property alias frameName: frameTitle.text
    signal titleChanged(string newTitle, string oldTitle)
    ColorSettings {
        id:colors
    }
    id: frame
    width: parent.width
    height: parent.height
    color: "#B3212121"
    radius: 2*scaleView
    border.color: hovered ? colors.node_hovered : selected ? colors.node_selected : "transparent"
    border.width: Math.max(scaleView, 1)

    function createNameInput() {
        var nameInput = Qt.createQmlObject('import QtQuick 2.12; TextInput{width: parent.width;
                                            horizontalAlignment: TextInput.AlignHCenter;
                                            color: "#C8C8C8";
                                            selectByMouse: true;
                                            selectionColor: "#7E7E7E"
                                            clip: true
                                            onFocusChanged: {if(!focus) accepted()}}',
                                           frame,
                                           "NodeFrame.qml");
        nameInput.y = Qt.binding(function(){return 10*scaleView;})
        nameInput.text = frameTitle.text;
        nameInput.font.pointSize = Qt.binding(function(){return 12*scaleView;})
        nameInput.leftPadding = Qt.binding(function(){return 5*scaleView;})
        nameInput.rightPadding = Qt.binding(function(){return 5*scaleView;});
        nameInput.focus = true
        frameTitle.visible = false;
        function labelUpdate() {
            if(frameTitle.text !== nameInput.text) frame.titleChanged(nameInput.text, frameTitle.text)
            frameTitle.text = nameInput.text;
            frameTitle.visible = true;
            nameInput.destroy()
        }

        nameInput.accepted.connect(labelUpdate)
    }

    onStartColorChanged: {
        frame.color = Qt.rgba(startColor.x, startColor.y, startColor.z, 0.7)
        titleBubbleRect.color = Qt.rgba(startColor.x, startColor.y, startColor.z, 1.0)
        tailOfBubble.requestPaint()
    }

    Item {
        id:bubble
        parent: frame.parent.parent
        x: (frame.width - width)*0.5 + frame.parent.x
        y: frame.parent.y - height
        z: 6
        width: textMetrics.tightBoundingRect.width + 30
        height: 50
        visible: bubbleVisible && scaleView < 0.6
        Rectangle {
            id: titleBubbleRect
            width: parent.width
            height: parent.height - 10
            radius: 3
            color: "#212121"
            Label {
                id: bubbleTitle
                leftPadding: 5
                rightPadding: 5
                text: frameTitle.text
                elide: Text.ElideRight
                font.pointSize: 16
                width: parent.width
                height: parent.height
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                color: "#C8C8C8"
            }
            TextMetrics {
                id: textMetrics
                text: bubbleTitle.text
                font:  bubbleTitle.font
            }
        }
        Canvas {
            id: tailOfBubble
            width: 6
            height: 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            contextType: "2d"
            onPaint: {
                var ctx = getContext("2d")
                ctx.reset();
                ctx.fillStyle = titleBubbleRect.color
                ctx.lineTo(6, 0)
                ctx.lineTo(3, 10)
                ctx.lineTo(0, 0)
                ctx.fill()
            }
        }
    }

    Rectangle {
        id: title
        width: parent.width - 2*Math.max(scaleView, 1)
        x: Math.max(scaleView, 1)
        y: Math.max(scaleView, 1)
        height: 35*scaleView
        color: "#801D1D1D"
        Label {
            id: frameTitle
            leftPadding: 5*scaleView
            rightPadding: 5*scaleView
            text: qsTr("Frame")
            elide: Text.ElideRight
            font.pointSize: 12*scaleView
            width: parent.width
            height: parent.height
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            color: "#C8C8C8"
        }
    }
}

