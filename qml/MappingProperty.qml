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
    id: mappingProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startInputMin: inputMinParam.propertyValue
    property alias startInputMax: inputMaxParam.propertyValue
    property alias startOutputMin: outputMinParam.propertyValue
    property alias startOutputMax: outputMaxParam.propertyValue
    signal inputMinChanged(real value)
    signal inputMaxChanged(real value)
    signal outputMinChanged(real value)
    signal outputMaxChanged(real value)
    ParamSlider {
        id: inputMinParam
        propertyName: "Input Min"
        onPropertyValueChanged: {
            inputMinChanged(inputMinParam.propertyValue)
        }
    }
    ParamSlider {
        id: inputMaxParam
        y: 33
        propertyName: "Input Max"
        onPropertyValueChanged: {
            inputMaxChanged(inputMaxParam.propertyValue)
        }
    }
    ParamSlider {
        id: outputMinParam
        y: 66
        propertyName: "Output Min"
        onPropertyValueChanged: {
            outputMinChanged(outputMinParam.propertyValue)
        }
    }
    ParamSlider {
        id: outputMaxParam
        y: 99
        propertyName: "Output Max"
        onPropertyValueChanged: {
            outputMaxChanged(outputMaxParam.propertyValue)
        }
    }
}
