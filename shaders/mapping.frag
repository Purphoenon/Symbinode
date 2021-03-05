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

#version 440 core
uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float inputMin = 0.0;
uniform float inputMax = 1.0;
uniform float outputMin = 0.0;
uniform float outputMax = 1.0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    vec3 outputValue = (texColor.rgb - vec3(inputMin))/vec3(inputMax - inputMin)*vec3(outputMax - outputMin)
                       + vec3(outputMin);
    vec4 result = vec4(outputValue, texColor.a);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
