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

import QtQuick 2.9
import Qt.labs.settings 1.1

Settings {
    property color node_selected: "#EE8738"
    property color node_hovered: "#FFC599"
    property color node_title_background: "#4A4A4A"
    property color node_content_background: "#E3212121"
    property var node_socket_background: ["#FFFF7700", "#FF52E220", "#FF0056A6", "#FFA86DB1", "#FFB54747",
                                          "#FFDBE220"]
    property color edge: "#001000"
    property color edge_selected: "#00ff00"
}
