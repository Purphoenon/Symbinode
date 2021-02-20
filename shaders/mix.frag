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

subroutine vec4 blendModeType(vec4 src, vec4 dst, float f);
subroutine uniform blendModeType blendMode;

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;
uniform sampler2D factorTexture;
uniform sampler2D maskTexture;
uniform float mixFactor = 0.5;
uniform bool useFactorTex = false;
uniform float backgroundOpacity = 1.0;
uniform float foregroundOpacity = 1.0;
uniform int mode = 0;
uniform bool includingAlpha = true;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

subroutine(blendModeType)
vec4 normal(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.a = src.a + dst.a*(1.0 - src.a);
    result.rgb = src.rgb*src.a + dst.rgb*dst.a*(1.0 - src.a);

    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 mixMode(vec4 src, vec4 dst, float f) {
    return mix(dst, src, f);
}

subroutine(blendModeType)
vec4 overlay(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.r = 2*dst.r*dst.a <= dst.a ? 2*src.r*src.a*dst.r*dst.a + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a) :
                                           src.r*src.a*(1.0 + dst.a) + dst.r*dst.a*(1.0 + src.a) - src.a*dst.a - 2*src.r*src.a*dst.r*dst.a;
    result.g = 2*dst.g*dst.a <= dst.a ? 2*src.g*src.a*dst.g*dst.a + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a) :
                                           src.g*src.a*(1.0 + dst.a) + dst.g*dst.a*(1.0 + src.a) - src.a*dst.a - 2*src.g*src.a*dst.g*dst.a;
    result.b = 2*dst.b*dst.a <= dst.a ? 2*src.b*src.a*dst.b*dst.a + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a) :
                                           src.b*src.a*(1.0 + dst.a) + dst.b*dst.a*(1.0 + src.a) - src.a*dst.a - 2*src.b*src.a*dst.b*dst.a;
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 screen(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = src.rgb*src.a + dst.rgb*dst.a - src.rgb*src.a*dst.rgb*dst.a;
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 soft_light(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    if(2*src.r*src.a <= src.a) {
        result.r = dst.r*dst.a*(src.a + (2*src.r*src.a - src.a)*(1.0 - dst.r)) + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a);
    }
    else if(2*src.r*src.a > src.a && 4*dst.r*dst.a <= dst.a) {
        result.r = dst.a*(2*src.r*src.a - src.a)*(16*dst.r*dst.r*dst.r - 12*dst.r*dst.r + 3*dst.r) + src.r*src.a - src.r*src.a*dst.a + dst.r*dst.a;
    }
    else if(2*src.r*src.a > src.a && 4*dst.r*dst.a > dst.a) {
        result.r = dst.a*(2*src.r*src.a - src.a)*(sqrt(dst.r) - dst.r) + src.r*src.a - src.r*src.a*dst.a + dst.r*dst.a;
    }

    if(2*src.g*src.a <= src.a) {
        result.g = dst.g*dst.a*(src.a + (2*src.g*src.a - src.a)*(1.0 - dst.g)) + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a);
    }
    else if(2*src.g*src.a > src.a && 4*dst.g*dst.a <= dst.a) {
        result.g = dst.a*(2*src.g*src.a - src.a)*(16*dst.g*dst.g*dst.g - 12*dst.g*dst.g + 3*dst.g) + src.g*src.a - src.g*src.a*dst.a + dst.g*dst.a;
    }
    else if(2*src.g*src.a > src.a && 4*dst.g*dst.a > dst.a) {
        result.g = dst.a*(2*src.g*src.a - src.a)*(sqrt(dst.g) - dst.g) + src.g*src.a - src.g*src.a*dst.a + dst.g*dst.a;
    }

    if(2*src.b*src.a <= src.a) {
        result.b = dst.b*dst.a*(src.a + (2*src.b*src.a - src.a)*(1.0 - dst.b)) + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a);
    }
    else if(2*src.b*src.a > src.a && 4*dst.b*dst.a <= dst.a) {
        result.b = dst.a*(2*src.b*src.a - src.a)*(16*dst.b*dst.b*dst.b - 12*dst.b*dst.b + 3*dst.b) + src.b*src.a - src.b*src.a*dst.a + dst.b*dst.a;
    }
    else if(2*src.b*src.a > src.a && 4*dst.b*dst.a > dst.a) {
        result.b = dst.a*(2*src.b*src.a - src.a)*(sqrt(dst.b) - dst.b) + src.b*src.a - src.b*src.a*dst.a + dst.b*dst.a;
    }

    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 hard_light(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    if(2*src.r*src.a <= src.a) {
        result.r = 2*src.r*src.a*dst.r*dst.a + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a);
    }
    else {
        result.r = src.r*src.a*(dst.a + 1.0) + dst.r*dst.a*(src.a + 1.0) - src.a*dst.a - 2*src.r*src.a*dst.r*dst.a;
    }

    if(2*src.g*src.a <= src.a) {
        result.g = 2*src.g*src.a*dst.g*dst.a + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a);
    }
    else {
        result.g = src.g*src.a*(dst.a + 1.0) + dst.g*dst.a*(src.a + 1.0) - src.a*dst.a - 2*src.g*src.a*dst.g*dst.a;
    }

    if(2*src.b*src.a <= src.a) {
        result.b = 2*src.b*src.a*dst.b*dst.a + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a);
    }
    else {
        result.b = src.b*src.a*(dst.a + 1.0) + dst.b*dst.a*(src.a + 1.0) - src.a*dst.a - 2*src.b*src.a*dst.b*dst.a;
    }

    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 lighten(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    /*if(src.r*dst.a > dst.r*src.a) {
        result.r = src.r*src.a + dst.r*dst.a*(1.0 - src.a);
    }
    else {
        result.r = dst.r*dst.a + src.r*src.a*(1.0 - dst.a);
    }
    if(src.g*dst.a > dst.g*src.a) {
        result.g = src.g*src.a + dst.g*dst.a*(1.0 - src.a);
    }
    else {
        result.g = dst.g*dst.a + src.g*src.a*(1.0 - dst.a);
    }
    if(src.b*dst.a > dst.b*src.a) {
        result.b = src.b*src.a + dst.b*dst.a*(1.0 - src.a);
    }
    else {
        result.b = dst.b*dst.a + src.b*src.a*(1.0 - dst.a);
    }*/
    result.rgb = max(src.rgb*src.a*dst.a, dst.rgb*dst.a*src.a) + src.rgb*src.a*(1.0 - dst.a) + dst.rgb*dst.a*(1.0 - src.a);
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 color_dodge(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    if(src.r*src.a == src.a && dst.r*dst.a == 0) {
        result.r = src.r*src.a*(1.0 - dst.a);
    }
    else if(src.r*src.a == src.a) {
        result.r = src.a*dst.a + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a);
    }
    else if(src.r*src.a < src.a) {
        result.r = src.a*dst.a*min(1, dst.r*dst.a/dst.a*src.a/(src.a - src.r*src.a)) + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a);
    }

    if(src.g*src.a == src.a && dst.g*dst.a == 0) {
        result.g = src.g*src.a*(1.0 - dst.a);
    }
    else if(src.g*src.a == src.a) {
        result.g = src.a*dst.a + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a);
    }
    else if(src.g*src.a < src.a) {
        result.g = src.a*dst.a*min(1, dst.g*dst.a/dst.a*src.a/(src.a - src.g*src.a)) + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a);
    }

    if(src.b*src.a == src.a && dst.b*dst.a == 0) {
        result.b = src.b*src.a*(1.0 - dst.a);
    }
    else if(src.b*src.a == src.a) {
        result.b = src.a*dst.a + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a);
    }
    else if(src.b*src.a < src.a) {
        result.b = src.a*dst.a*min(1, dst.b*dst.a/dst.a*src.a/(src.a - src.b*src.a)) + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a);
    }
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 color_burn(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    if(src.r*src.a == 0 && dst.r*dst.a == dst.a) {
        result.r = src.a*dst.a + (dst.r*dst.a)*(1.0 - src.a);
    }
    else if(src.r*src.a == 0) {
        result.r = dst.r*dst.a*(1.0 - src.a);
    }
    else if(src.r*src.a > 0) {
        result.r = src.a*dst.a*(1.0 - min(1, (1.0 - dst.r)*1.0/src.r)) + src.r*src.a*(1.0 - dst.a) + dst.r*dst.a*(1.0 - src.a);
    }

    if(src.g*src.a == 0 && dst.g*dst.a == dst.a) {
        result.g = src.a*dst.a + (dst.g*dst.a)*(1.0 - src.a);
    }
    else if(src.g*src.a == 0) {
        result.g = dst.g*dst.a*(1.0 - src.a);
    }
    else if(src.g*src.a > 0) {
        result.g = src.a*dst.a*(1.0 - min(1, (1.0 - dst.g)*1.0/src.g)) + src.g*src.a*(1.0 - dst.a) + dst.g*dst.a*(1.0 - src.a);
    }

    if(src.b*src.a == 0 && dst.b*dst.a == dst.a) {
        result.b = src.a*dst.a + (dst.b*dst.a)*(1.0 - src.a);
    }
    else if(src.b*src.a == 0) {
        result.b = dst.b*dst.a*(1.0 - src.a);
    }
    else if(src.b*src.a > 0) {
        result.b = src.a*dst.a*(1.0 - min(1, (1.0 - dst.b)*1.0/src.b)) + src.b*src.a*(1.0 - dst.a) + dst.b*dst.a*(1.0 - src.a);
    }

    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 darken(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = min(src.rgb*src.a*dst.a, dst.rgb*dst.a*src.a) + src.rgb*src.a*(1.0 - dst.a) + dst.rgb*dst.a*(1.0 - src.a);
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 add(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = min(dst.rgb*dst.a + src.rgb*src.a, vec3(1.0));
    result.a = min(dst.a + src.a, 1.0);
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 subtract(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = max(src.rgb*src.a*(1.0 - 2.0*dst.a) + dst.rgb*dst.a, vec3(0.0));
    result.a = max(src.a + dst.a - src.a*dst.a, 0.0);
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 multiply(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = dst.rgb*dst.a*src.rgb*src.a + src.rgb*src.a*(1.0 - dst.a) + dst.rgb*dst.a*(1.0 - src.a);
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 divide(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = dst.rgb*dst.a/src.rgb*src.a + src.rgb*src.a*(1.0 - dst.a) + dst.rgb*dst.a*(1.0 - src.a);
    if(src.a == 0.0) result.rgb = dst.rgb;
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 difference(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = src.rgb*src.a + dst.rgb*dst.a - 2*min(src.rgb*src.a, dst.rgb*dst.a);
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

subroutine(blendModeType)
vec4 exclusion(vec4 src, vec4 dst, float f) {
    vec4 result = vec4(0.0);
    result.rgb = (src.rgb*src.a*dst.a + dst.rgb*dst.a*src.a - 2*src.rgb*src.a*dst.rgb*dst.a) + src.rgb*src.a*(1.0 - dst.a) + dst.rgb*dst.a*(1.0 - src.a);
    result.a = src.a + dst.a - src.a*dst.a;
    if(src.a > 0) {
        result.a = result.a*f + dst.a*(1.0 - f);
        result.rgb = (result.rgb*f + dst.rgb*dst.a*(1.0 - f))/result.a;
    }
    else result = dst;
    return result;
}

void main()
{
    float factor = useFactorTex ? texture(factorTexture, texCoords).r : mixFactor;
    vec4 fTex = texture(firstTexture, texCoords);
    vec4 sTex = texture(secondTexture, texCoords);
    vec4 result = vec4(0.0);

    fTex.a *= backgroundOpacity;
    sTex.a *= foregroundOpacity;

    result = blendMode(sTex, fTex, factor);

    if(!includingAlpha) {
        if(result.a > 0) {
            result.rgb *= result.a;
            result.a = 1.0;
        }
        else {
            result = vec4(0, 0, 0, 1);
        }
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }

    FragColor = result;
}
