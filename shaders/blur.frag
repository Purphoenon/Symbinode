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

#define PI 3.14159265359

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float intensity = 0.5;
uniform vec2 resolution;
uniform vec2 direction;
uniform bool useMask = false;
vec2 size = 0.001*resolution*intensity;

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
    /*float sum = 0.0;
    float sigma = max(0.001, radius);
    float g = gauss(0.0, sigma);
    color = texture(sourceTexture, vec2(gl_FragCoord) / resolution)*g;
    sum += g;

    for (int i=1; i<64; i++) {
        g = gauss(i, sigma);
        color += texture(sourceTexture, (gl_FragCoord.xy + i*direction) / resolution )*g;
        color += texture(sourceTexture, (gl_FragCoord.xy - i*direction) / resolution )*g;
        sum += g + g;
    }

    color /= sum;*/

    color = texture(sourceTexture, vec2(gl_FragCoord) / resolution) * weight[0];
    for (int i=1; i<3; i++) {
        color +=
            texture(sourceTexture, (vec2(gl_FragCoord) + direction*offset[i]*size) / resolution)
                * weight[i];
        color +=
            texture(sourceTexture, (vec2(gl_FragCoord) - direction*offset[i]*size) / resolution)
                * weight[i];
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor  = color;
}
