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

in vec2 texCoords;
out vec4 FragColor;

void main(void)
{
    float total = floor(texCoords.x * 16) + floor(texCoords.y * 16);
    bool isEven = mod(total,2.0)==0.0;
    vec4 col1 = vec4(0.6,0.6,0.6,1.0);
    vec4 col2 = vec4(1.0,1.0,1.0,1.0);
    FragColor = isEven ? col1 : col2;
}
