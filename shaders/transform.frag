#version 440 core
#define PI 3.14159265359

uniform sampler2D transTexture;
uniform sampler2D maskTexture;
uniform vec2 translate = vec2(0.0, 0.0);
uniform vec2 scale = vec2(1.0, 1.0);
uniform int angle = 0;
uniform bool clampTrans = false;
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
    coords += translate;
    coords -= 0.5;
    coords *= rotate2d(angle*PI/180.0);
    coords *= vec2(1.0)/scale;
    coords += 0.5;
    coords = clampTrans ? clamp(coords, vec2(0.0), vec2(1.0)) : coords;
    vec4 result = texture(transTexture, coords);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
