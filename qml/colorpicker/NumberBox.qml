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

Item {
    property alias name: label.text
    property alias numberBox: textBox
    property real maximumValue: 360
    property real offsetX: 14
    property real boxWidth: 25
    Label {
        id: label
        text: "H"
        y: 4
        color: "#E0E0E0"
        font.pointSize: 8
        font.bold: true
    }

    Rectangle {
        width: boxWidth + 5
        height: 17
        radius: 3
        x: offsetX - 5
        y: 2
        border.width: 1
        border.color: "#6B737B"
        color: "transparent"
    }

    TextInput {
        id: textBox
        width: boxWidth
        height: 20
        x: offsetX
        color: "#E0E0E0"
        font.pointSize: 8
        selectByMouse: true
        validator: IntValidator{bottom: 0; top: maximumValue;}
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        onFocusChanged: {
            if(!focus) accepted()
        }
        onAccepted: {
            focus = false
        }
    }
}
