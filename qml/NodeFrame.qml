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
    property alias frameName: frameTitle.text
    signal titleChanged(string newTitle, string oldTitle)
    ColorSettings {
        id:colors
    }
    id: frame
    width: parent.width
    height: parent.height
    color: "#E3212121"
    radius: 2*scaleView
    border.color: hovered ? colors.node_hovered : selected ? colors.node_selected : "transparent"
    border.width: Math.max(scaleView, 1)

    function createNameInput() {
        var nameInput = Qt.createQmlObject('import QtQuick 2.12; TextInput{width: parent.width;
                                            horizontalAlignment: TextInput.AlignHCenter;
                                            color: "#C8C8C8";
                                            selectByMouse: true;
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

    Label {
        id: frameTitle
        y: 10*scaleView
        leftPadding: 5*scaleView
        rightPadding: 5*scaleView
        text: qsTr("Frame")
        elide: Text.ElideRight
        font.pointSize: 12*scaleView
        width: parent.width
        horizontalAlignment: TextInput.AlignHCenter
        color: "#C8C8C8"
    }
}

