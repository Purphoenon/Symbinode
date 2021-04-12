#version 440 core

#define PI 3.14159265359

uniform sampler2D sourceTexture;
uniform sampler2D warpTexture;
uniform sampler2D maskTexture;
uniform float intensity = 0.1f;
uniform int angle = 0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}

void main()
{
    vec2 dir = rotate2d(PI*angle/180)*vec2(-1.0, 0.0);
    vec2 r = vec2(texture(warpTexture, texCoords).x, texture(warpTexture, texCoords + vec2(5.2, 1.3)).x);
    vec2 warp = vec2(texture(warpTexture, texCoords + intensity*r + vec2(1.7, 9.2)).x,
                     texture(warpTexture, texCoords + intensity*r + vec2(8.3, 2.8)).x);
    vec4 result = texture(sourceTexture, texCoords + dir*warp*intensity);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
