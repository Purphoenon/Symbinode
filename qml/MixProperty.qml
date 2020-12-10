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
    height: childrenRect.height + 30
    width: parent.width
    property alias startMode: control.currentIndex
    property real startFactor
    signal modeChanged(int mode)
    signal factorChanged(real f)

    ParamDropDown {
        id: control
        y: 15
        model: ["Mix", "Add", "Multiply", "Substract", "Divide"]
        onActivated: {
            if(index == 0) {
                modeChanged(0)
            }
            else if(index == 1) {
                modeChanged(1)
            }
            else if(index == 2) {
                modeChanged(2)
            }
            else if(index == 3) {
                modeChanged(3)
            }
            else if(index == 4) {
                modeChanged(4)
            }
            focus = false
        }
    }

    ParamSlider {
        id: factorParam
        y: 38
        propertyName: "Factor"
        propertyValue: startFactor
        onPropertyValueChanged: {
            factorChanged(factorParam.propertyValue)
        }
    }
}
