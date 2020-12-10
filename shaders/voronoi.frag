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
subroutine float voronoiType(vec2 st);
subroutine uniform voronoiType voronoiFunction;
uniform sampler2D maskTexture;
uniform vec2 res;
uniform int scale = 5;
uniform int scaleX = 1;
uniform int scaleY = 1;
uniform float jitter = 1.0;
uniform bool inverse = false;
uniform float intensity = 2.0;
uniform float bordersSize = 0.0;
uniform bool useMask = false;

out vec4 FragColor;

vec2 random2( vec2 p ) {
    p = mod(p, scale*vec2(scaleX, scaleY));
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec2 f2Voronoi(vec2 st) {
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float f1 = 1.0;
    float f2 = 1.0;

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x),float(y));
            vec2 point = random2(i_st + neighbor)* jitter;
            vec2 diff = neighbor + point - f_st;
            float dist = dot(diff, diff);

            if( dist < f1) {
                f2 = f1;
                f1 = dist;
            }
            else if(dist < f2) {
                f2 = dist;
            }
        }
    }
    return vec2(f1, f2);
}

float distanceVoronoi(vec2 st) {
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    vec2 mb;
    vec2 mr;

    float res = 8.0;
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2 b = vec2(i, j);
        vec2  r = vec2(b) + random2(i_st+b)*jitter-f_st;
        float d = dot(r,r);

        if( d < res )
        {
            res = d;
            mr = r;
            mb = b;
        }
    }

    res = 8.0;
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        vec2 b = mb + vec2(i, j);
        vec2  r = vec2(b) + random2(i_st+b)*jitter - f_st;
        float d = dot(0.5*(mr+r), normalize(r-mr));

        if ( dot(mr-r,mr-r)>0.00001 ) {
            res = min(res, dot( 0.5*(mr+r), normalize(r-mr) ));
        }
    }
    return res;
}

vec2 voronoi(vec2 st) {
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.0;
    vec2 m_point;

    float u = clamp(jitter, 0.001, 1.0);
    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 point = random2(i_st + neighbor)*u;
            vec2 diff = neighbor + point - f_st;
            float dist = dot(diff, diff);          

            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }
    return m_point/u;
}

subroutine(voronoiType)
float worley(vec2 st) {
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float f1 = 1.0;

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x),float(y));
            vec2 point = random2(i_st + neighbor)*jitter;
            vec2 diff = neighbor + point - f_st;
            float dist = dot(diff, diff);

            f1 = min(f1, dist);
        }
    }
    return f1;
}

subroutine(voronoiType)
float crystals(vec2 st) {
    vec2 F2 = f2Voronoi(st);
    return 0.1 + (F2.y - F2.x);
}

subroutine(voronoiType)
float borders(vec2 st) {
    float w = bordersSize*0.1;
    return 1.0 - smoothstep( w, w + 0.02, distanceVoronoi(st));
}

subroutine(voronoiType)
float solid(vec2 st) {
    return dot(voronoi(st),vec2(.3,.6));
}

void main(void)
{
    vec2 st = gl_FragCoord.xy/res;
    st.x *= res.x/res.y;
    vec3 color = vec3(0.0);
    color += voronoiFunction(st*scale*vec2(scaleX, scaleY));
    color *= intensity;
    color = inverse ? 1.0 - color : color;
    vec4 result = vec4(color, 1.0);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, st);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result.rgb *= mask;
        result.a = mask;
    }
    FragColor = result;
}
