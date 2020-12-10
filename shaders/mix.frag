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

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;
uniform sampler2D factorTexture;
uniform sampler2D maskTexture;
uniform float mixFactor = 0.5;
uniform bool useFactorTex = false;
uniform int mode = 0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    float factor = useFactorTex ? texture(factorTexture, texCoords).r : mixFactor;
    vec4 fTex = texture(firstTexture, texCoords);
    vec4 sTex = texture(secondTexture, texCoords);
    vec4 result = vec4(0.0);

    if(mode == 0) {
        result = mix(fTex, sTex, factor);
    }
    else if(mode == 1) {
        result = (fTex + sTex)*factor + fTex*(1.0 - factor);
    }
    else if(mode == 2) {
        result = fTex*sTex*factor + fTex*(1.0 - factor);
    }
    else if(mode == 3) {
        result = (fTex - sTex)*factor + fTex*(1.0 - factor);
    }
    else if(mode == 4) {
        result = (fTex/(sTex + 0.0039))*factor + fTex*(1.0 - factor);
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }

    FragColor = result;
}
