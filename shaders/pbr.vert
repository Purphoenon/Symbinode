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
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 Pos;
out vec3 camPos;
out vec3 tangentFragPos;
out vec3 tangentView;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 cameraPos;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(model) * aNormal);
    Tangent = normalize(mat3(model) * aTangent);
    Bitangent = normalize(cross(Normal, Tangent));
    mat3 TBN = transpose(mat3(Tangent, Bitangent, Normal));
    tangentView = TBN*cameraPos;
    tangentFragPos = TBN*WorldPos;

    Pos = aPos;
    camPos = cameraPos;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
