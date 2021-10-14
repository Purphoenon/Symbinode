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

uniform vec2 res;
uniform sampler2D grayscaleTexture;
uniform float strength;

vec2 size = 0.001*res;
out vec4 FragColor;

void main() {
    vec2 st = gl_FragCoord.xy/res;
    st.x *= res.x/res.y;
    vec2 offset = vec2(1.0)/res*size;

    float dx = 0.0;
    float dy = 0.0;
    float tl = texture(grayscaleTexture, st + vec2(-offset.x, offset.y)).x;
    float t = texture(grayscaleTexture, st + vec2(0.0, offset.y)).x;
    float tr = texture(grayscaleTexture, st + vec2(offset.x, offset.y)).x;
    float bl = texture(grayscaleTexture, st + vec2(-offset.x, -offset.y)).x;
    float b = texture(grayscaleTexture, st + vec2(0.0, -offset.y)).x;
    float br = texture(grayscaleTexture, st + vec2(offset.x, -offset.y)).x;
    float l = texture(grayscaleTexture, st + vec2(-offset.x, 0.0)).x;
    float r = texture(grayscaleTexture, st + vec2(offset.x, 0.0)).x;
    float c = texture(grayscaleTexture, st + vec2(0.0, 0.0)).x;

    float top_side = tl + 2.0*t + tr;
    float bottom_side = bl + 2.0*b + br;
    float right_side = tr + 2.0*r + br;
    float left_side = tl + 2.0*l + bl;
    dx = right_side - left_side;
    dy = bottom_side - top_side;

    float s = strength == 0 ? 0.05 : strength;
    vec3 norm = normalize(vec3(dx, dy, 1.0/abs(s))) * 0.5 + 0.5;
    if(strength > 0) {
       norm.y = 1.0 - norm.y;
    }

    FragColor = vec4(norm, 1.0);
}
