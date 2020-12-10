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
    id: brightnessContrastProp
    height: childrenRect.height + 30
    width: parent.width
    property alias startBrightness: brightnessParam.propertyValue
    property alias startContrast: contrastParam.propertyValue
    signal brightnessChanged(real value)
    signal contrastChanged(real value)
    ParamSlider {
        id: brightnessParam
        propertyName: "Brightness"
        minimum: -1
        onPropertyValueChanged: {
            brightnessChanged(brightnessParam.propertyValue)
        }
    }
    ParamSlider {
        id: contrastParam
        y: 33
        propertyName: "Contrast"
        minimum: -1
        onPropertyValueChanged: {
            contrastChanged(contrastParam.propertyValue)
        }
    }
}
