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
#define PI 3.14159265359
uniform float offsetX = 0.0f;
uniform float offsetY = 0.0f;
uniform int columns = 5;
uniform int rows = 5;
uniform float scaleX = 1.0;
uniform float scaleY = 1.0;
uniform float scale = 1.0;
uniform int rotationAngle = 0;
uniform float randPosition = 0.0;
uniform float randRotation = 0.0;
uniform float randScale = 0.0;
uniform float maskStrength = 0.0;
uniform bool keepProportion = false;
uniform bool useAlpha = true;
uniform bool depthMask = true;
uniform int inputCount = 1;
uniform bool useMask = false;
uniform sampler2D textureTile;
uniform sampler2D tile1;
uniform sampler2D tile2;
uniform sampler2D tile3;
uniform sampler2D tile4;
uniform sampler2D tile5;
uniform sampler2D randomTexture;
uniform sampler2D maskTexture;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main()
{
    vec2 coords = texCoords;
    vec2 scaledUV = coords*vec2(columns, rows);
    vec2 offsetForProportion = vec2(0, 0);
    vec2 scaleForProportion = vec2(1.0, 1.0);
    if(keepProportion) {
        if(columns < rows) {
            offsetForProportion = vec2(1.0 - columns/rows, 0.0)*0.5;
            scaleForProportion = vec2(float(rows)/float(columns), 1.0);
        }
        else {
            offsetForProportion = vec2(0.0, 1.0 - rows/columns)*0.5;
            scaleForProportion = vec2(1.0, float(columns)/float(rows));
        }
    }
    vec2 cell = floor(scaledUV);
    vec2 offset = fract(scaledUV);
    vec2 randomUV = cell*vec2(0.037, 0.119);
    vec4 random = texture(randomTexture, randomUV);
    float priority = -1.0;
    vec4 image;
    vec4 color = vec4(0.0);
    for(int j = -3; j <= 3; ++j) {
        for(int i = -3; i <= 3; ++i) {
            vec2 cell_t = cell + vec2(i, j);
            cell_t = mod(cell_t, vec2(columns, rows));
            vec2 offset_t = offset - vec2(i, j);
            randomUV = cell_t*vec2(0.037, 0.119);
            random = texture(randomTexture, randomUV);
            randomUV += vec2(0.03, 0.17);

            vec2 coords = (offset_t - vec2(mod(cell_t.y, 2.0)*offsetX, mod(cell_t.x, 2.0)*offsetY) +
                           random.xy*randPosition);
            if(keepProportion) {
                coords -= offsetForProportion;
                coords *= scaleForProportion;
                coords += offsetForProportion;
            }
            coords -= 0.5;
            coords *= rotate2d(rotationAngle*PI/180.0 + PI*random.z*randRotation);
            coords /= (vec2(scaleX, scaleY) - vec2(random.w*randScale));
            coords /= scale;
            coords += 0.5;
            if((coords.x < 0.0 || coords.y < 0.0 || coords.x > 1.0 || coords.y > 1.0)) continue;
            int textureNumber = int(floor((random.z + random.w)*0.5*(inputCount - 0.1)));
            if(textureNumber == 0) {
                image = textureLod(textureTile, coords, 0);
            }
            else if(textureNumber == 1) {
                image = textureLod(tile1, coords, 0);
            }
            else if(textureNumber == 2) {
                image = textureLod(tile2, coords, 0);
            }
            else if(textureNumber == 3) {
                image = textureLod(tile3, coords, 0);
            }
            else if(textureNumber == 4) {
                image = textureLod(tile4, coords, 0);
            }
            else if(textureNumber == 5) {
                image = textureLod(tile5, coords, 0);
            }
            float maskS = 1.0 - (random.x + random.y)*0.5*maskStrength;

            if(image.a > 0) {
                if(maskS > priority && depthMask)
                {
                    if(image.a < 1.0) {
                        float a = image.a + color.a*(1 - image.a);
                        vec3 rgb = (image.rgb*maskS*image.a + color.rgb*color.a*(1 - image.a))/a;
                        color = vec4(rgb, a);
                    }
                    else {
                        color.a = image.a;
                        color.rgb = image.rgb*maskS;
                    }
                    priority = maskS;
                }
                else {
                    if(color.a < 1.0) {
                        float a = color.a + image.a*(1.0 - color.a);
                        vec3 rgb = (color.rgb*color.a + image.rgb*maskS*image.a*(1 - color.a))/a;
                        color = vec4(rgb, a);
                    }
                }
            }
        }
    }

    if(!useAlpha) color.a = 1.0;

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }
    FragColor = color;
}
