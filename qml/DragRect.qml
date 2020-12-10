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
    id: node
    property real baseX: 40
    property real baseY: 50
    property real panX: 0
    property real panY: 0
    property bool selected: false
    property bool hovered: false
    width: 80; height: 100
    transformOrigin: Item.TopLeft
    border.color: node.hovered ? colors.node_hovered : node.selected ? colors.node_selected : "black"
    border.width: 1
    radius: node.radius
    color: node.color
 }


