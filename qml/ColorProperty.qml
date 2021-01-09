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
import "colorpicker/"

Item {
    id: colorProp
    height: parent.parent.height
    width: parent.width
    property vector3d startColor: Qt.vector3d(1.0, 1.0, 1.0)
    signal colorChanged(vector3d color)
    signal propertyChangingFinished(string name, var newValue, var oldValue)
    ColorPicker {
        id: color
        height: parent.parent.height - 15
        width: parent.width
        startingColor: Qt.rgba(startColor.x, startColor.y, startColor.z, 1.0)
        onColorValueChanged: {
            colorChanged(Qt.vector3d(colorValue.r, colorValue.g, colorValue.b))
        }
        onColorChangingFinished: {
            propertyChangingFinished("startColor", Qt.vector3d(colorValue.r, colorValue.g, colorValue.b), Qt.vector3d(oldColor.r, oldColor.g, oldColor.b))
        }
    }
}
