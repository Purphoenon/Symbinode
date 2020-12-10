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
uniform float brightness = 0.5;
uniform float contrast = 0.5;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    vec3 color = texColor.rgb;
    color = (color - 0.5)*(contrast + 1.0) + 0.5;
    color += brightness;
    FragColor = vec4(color, texColor.a);
}
