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

ComboBox {
    property color popupColor: "#2C2D2F"
    property int oldIndex
    id: control
    model: ["Perlin", "Simple"]
    leftPadding: 10
    rightPadding: 10
    width: parent.width - 10
    height: childrenRect.height

    onPressedChanged: {
        if(pressed) oldIndex = currentIndex
    }

    delegate: ItemDelegate {
        width: control.width
        height: 25
        contentItem: Text {
            text: modelData
            color: "#B2B2B2"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex === index
        background: Rectangle {
            color: parent.highlighted ? "#404347" : popupColor
        }
    }

    indicator: Canvas {
            id: canvas
            x: control.width - width - control.rightPadding
            y: control.topPadding + (control.availableHeight - height) / 2 + 0.5
            width: 6
            height: 4
            contextType: "2d"

            Connections {
                target: control
                function onPressedChanged() { canvas.requestPaint(); }
            }

            onPaint: {
                var context = getContext("2d");
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = "#B2B2B2"
                context.fill();
            }
        }

    contentItem: Text {
        topPadding: 6
        leftPadding: 12
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        color: "#B2B2B2"
        //verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        x: parent.leftPadding
        y: parent.topPadding
        width: parent.width - parent.rightPadding
        height: 25
        color: "#484C51"
        border.width: 1
        border.color: "#2C2D2F"
        radius: 3
    }

    popup: Popup {
        x: parent.leftPadding
        y: control.height - 2 + control.topPadding
        width: control.width - parent.rightPadding
        implicitHeight: Math.min(contentHeight, 200) + topPadding
        padding: 0
        topPadding: 2

        contentItem: ListView {
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollBar.vertical: ScrollBar{
                active: popup.contentHeight > 200
                 z: 1
            }
        }

        background: Rectangle {
            color: popupColor
            radius: 2
        }
    }
}
