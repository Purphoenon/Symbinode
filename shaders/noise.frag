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

subroutine float noiseType(vec2 st, vec2 size);
subroutine uniform noiseType noise;
uniform sampler2D maskTexture;
uniform vec2 res;
uniform float scale = 5.0;
uniform int octaves = 8;
uniform float persistence = 0.5;
uniform float amplitude = 1.0;
uniform float scaleX = 1.0;
uniform float scaleY = 1.0;
uniform int seed = 1;
uniform bool useMask = false;
out vec4 FragColor;

float random (in vec2 st) {
    return - 1.0 + 2.0*fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 1367.454541*seed);
}

vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return (-1.0 + 2.0*fract(sin(st)*1367.454541*seed));
}
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float getWrappedValue(vec2 corner, vec2 height) {
    if(corner.x == height.x) {
         corner.x = 0;
    } if(corner.y == 0 || corner.y == height.y) {
         corner.x -= height.x;
    }
    if(corner.y == height.y) {corner.y = 0;}

    float gradient = random(corner);

    return gradient;
}

vec2 getWrappedPerlinGradient(vec2 corner, vec2 height) {

    // We'll use this to track whether we're sampling a mirrored point.
    float flipY = 1.0;

    if(corner.x == height.x) {
         corner.x = 0;
    } if(corner.y == 0 || corner.y == height.y) {
         corner.x -= height.x;
         flipY = -1.0;
    }
    if(corner.y == height.y) {corner.y = 0;}

    vec2 gradient = random2(corner);

    gradient.y *= flipY;
    return gradient;
}

subroutine(noiseType)
float noiseSimple (vec2 st, vec2 s) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = getWrappedValue(i, s);
    float b = getWrappedValue(i + vec2(1.0, 0.0), s);
    float c = getWrappedValue(i + vec2(0.0, 1.0), s);
    float d = getWrappedValue(i + vec2(1.0, 1.0), s);

    vec2 u = f*f*(3.0-2.0*f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

subroutine(noiseType)
float noisePerlin(vec2 st, vec2 s) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( getWrappedPerlinGradient(i + vec2(0.0,0.0), s), f - vec2(0.0,0.0) ),
                     dot( getWrappedPerlinGradient(i + vec2(1.0,0.0), s), f - vec2(1.0,0.0) ), u.x),
                mix( dot( getWrappedPerlinGradient(i + vec2(0.0,1.0), s), f - vec2(0.0,1.0) ),
                     dot( getWrappedPerlinGradient(i + vec2(1.0,1.0), s), f - vec2(1.0,1.0) ), u.x), u.y);
}

float noiseSimplex(vec2 v) {
    const vec4 C = vec4(0.211324865405187,
                        0.366025403784439,
                        -0.577350269189626,
                        0.024390243902439);

    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    vec2 i1 = vec2(0.0);
    i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
    vec2 x1 = x0.xy + C.xx - i1;
    vec2 x2 = x0.xy + C.zz;

    i = mod289(i);
    vec3 p = permute(
            permute( i.y + vec3(0.0, i1.y, 1.0))
                + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(
                        dot(x0,x0),
                        dot(x1,x1),
                        dot(x2,x2)
                        ), 0.0);

    m = m*m ;
    m = m*m ;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

    vec3 g = vec3(0.0);
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
    return 130.0 * dot(m, g);
}

void main() {
   float frequency = floor(scale);
   float lacunarity = 2.0;
   float ampl = amplitude;
   vec2 st = gl_FragCoord.xy/res;
   st.x *= res.x/res.y;
   vec2 s = vec2(scaleX, scaleY);
   vec3 color = vec3(0.0);

   for(int i = 0; i <= octaves; ++i) {
      color += ampl*noise(st*frequency*s, frequency*s);
      frequency *= lacunarity;
      ampl *= persistence;
   }

   color =  0.5 + color*0.5;

   vec4 result = vec4(color, 1.0);
   if(useMask) {
       vec4 maskColor = texture(maskTexture, st);
       float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
       result *= mask;
   }

   FragColor = result;
}
