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

#version 440
#define PI 3.14159265359
uniform sampler2D sourceTexture;
uniform sampler2D randomTexture;
uniform sampler2D maskTexture;
uniform int imagePerCell = 3;
uniform int scale = 4;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main()
{
    vec2 coords = texCoords;
    vec2 scaledUV = coords*vec2(4, 6);
    vec2 cell = floor(scaledUV);
    vec2 offset = fract(scaledUV);
    vec2 randomUV = cell*vec2(0.037, 0.119);
    vec4 random = texture(randomTexture, randomUV);
    vec4 image;
    vec4 color = vec4(0.0);
    for(int i = -3; i <= 2; ++i) {
        for(int j = -4; j <= 2; ++j) {
            vec2 cell_t = cell + vec2(i, j);
            vec2 offset_t = offset - vec2(i, j);
            randomUV = cell_t*vec2(0.037, 0.119);
            random = texture(randomTexture, randomUV);
            randomUV += vec2(0.03, 0.17);
            vec2 coords = rotate2d(0.0*PI/180.0 + PI*random.z*0.0)*(offset_t - vec2(mod(cell_t.y, 2.0)*0.0, mod(cell_t.x, 2.0)*0.0) + random.xy*0.0)/(vec2(1.0, 1.0) - vec2(random.w*0.0));
            coords -= vec2(1.0/3.0, 0.0) * 0.5;
            coords *= vec2(6.0/4.0, 1.0);
            if((coords.x < 0.0 || coords.y < 0.0 || coords.x > 1.0 || coords.y > 1.0)) continue;
            image = texture(sourceTexture, coords);
            float maskS = 1.0 - (random.x + random.y)*0.5*0.0;

            if(image.a > 0) {
                image.rgb = max(color.rgb, image.rgb*maskS);
                color = image*image.a + color*(1.0 - image.a);
            }


            /*for(int k = 0; k < imagePerCell; ++k) {
                random = texture(randomTexture, randomUV);
                randomUV += vec2(0.03, 0.17);
                //if((random.x+random.y)*0.5 < 0.6) continue;
                vec2 coords = rotate2d(random.z*PI)*(offset_t - random.xy)/vec2(0.5*random.w + 0.5);
                if((coords.x < 0.0 || coords.y < 0.0 || coords.x > 1.0 || coords.y > 1.0)) continue;
                image = texture(sourceTexture, coords);
                if(image.a > 0) {
                   color = image*image.a + color*(1.0 - image.a);
                }
            }*/
        }
    }
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }
    FragColor = color;
}
