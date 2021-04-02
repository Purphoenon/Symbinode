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
uniform float intensity = 0.5;
uniform vec2 resolution;
uniform vec2 direction;
uniform bool useMask = false;
vec2 size = 0.003*resolution*intensity;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

in vec2 texCoords;

out vec4 FragColor;

float gauss (float x, float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    vec4 color = vec4(0.0);
    /*float total = 0.0;
    float g = gauss(0, 5.0);
    vec4 texColor = texture(sourceTexture, texCoords);
    color += texColor*g;
    total += g;
    for(int i = 1; i < 10; ++i) {
        float w = gauss(i, 5.0);
        total += 2*w;
        color += w * texture(sourceTexture, texCoords + (direction*i/resolution)*size);
        color += w * texture(sourceTexture, texCoords - (direction*i/resolution)*size);
    }

    color /= total;*/
    color = texture2D(sourceTexture, vec2(gl_FragCoord) / resolution) * weight[0];
    for (int i=1; i<3; i++) {
        color +=
            texture2D(sourceTexture, (vec2(gl_FragCoord) + direction*offset[i]*size) / resolution)
                * weight[i];
        color +=
            texture2D(sourceTexture, (vec2(gl_FragCoord) - direction*offset[i]*size) / resolution)
                * weight[i];
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor  = color;
}
