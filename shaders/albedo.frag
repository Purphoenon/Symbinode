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

uniform sampler2D albedoTex;
uniform bool useAlbedoTex = false;
uniform vec3 albedoVal;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 albedoTex = texture(albedoTex, texCoords);
    vec4 albedo = vec4(albedoVal, 1.0);
    if(useAlbedoTex) {
        albedo = albedoTex*albedoTex.a + (1.0 - albedoTex.a)*albedo;
    }
    FragColor = vec4(albedo.rgb, 1.0);
}
