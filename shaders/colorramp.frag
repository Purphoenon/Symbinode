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
struct stopGradient {
    vec4 color;
    float pos;
};

layout (std430, binding = 0) buffer stops {
    vec4 gradients[];
};

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform int stopCount;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    float x = (texColor.r + texColor.g + texColor.b)/3.0f;
    vec3 color = stopCount > 0 ? gradients[0].rgb : vec3(1.0f);
    for(int i = 1; i < stopCount; ++i) {
        if((x > gradients[i - 1].w) && (x <= gradients[i].w)) {
            float f = (x - gradients[i - 1].w)/(gradients[i].w - gradients[i - 1].w);
            color = mix(gradients[i - 1].rgb, gradients[i].rgb, f);
            break;
        }
    }
    if(x > gradients[stopCount - 1].w) {
        color = gradients[stopCount - 1].rgb;
    }
    vec4 result = vec4(color, texColor.a);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
