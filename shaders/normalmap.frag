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
    vec2 offsets[9] = vec2[](
         vec2(-offset.x,  offset.y), // top-left
         vec2( 0.0f,    offset.y), // top-center
         vec2( offset.x,  offset.y), // top-right
         vec2(-offset.x,  0.0f),   // center-left
         vec2( 0.0f,    0.0f),   // center-center
         vec2( offset.x,  0.0f),   // center-right
         vec2(-offset.x, -offset.y), // bottom-left
         vec2( 0.0f,   -offset.y), // bottom-center
         vec2( offset.x, -offset.y)  // bottom-right
     );
    float sobelX[9] = float[](-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1);
    float sobelY[9] = float[](-1, -2, -1,
                               0,  0,  0,
                               1,  2,  1);

    float dx = 0.0;
    float dy = 0.0;

    for(int i = 0; i < 9; ++i) {
        dx += sobelX[i] * texture(grayscaleTexture, st + offsets[i]).r;
        dy += sobelY[i] * texture(grayscaleTexture, st + offsets[i]).r;
    }
    float s = strength == 0 ? 0.05 : strength;
    vec3 norm = normalize(vec3(dx, dy, 1.0/abs(s))) * 0.5 + 0.5;
    if(strength < 0) {
       norm.y = 1.0 - norm.y;
    }
    FragColor = vec4(norm, 1.0);
}
