#version 440 core
#define PI 3.14159265359

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float radius = 2.0;
uniform int angle = 0;
uniform bool useClamp = false;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main()
{
    vec2 pos = vec2(0.5) - texCoords;
    float r = length(pos)*radius;
    pos *= rotate2d((angle*PI)/180);
    float a = ((atan(pos.y, pos.x))/PI + 1.0)*0.5;
    vec2 coord = useClamp ? clamp(vec2(a, r), vec2(0.0), vec2(1.0)) : vec2(a, r);
    if(useClamp) {
        if(a < 0.0 || r < 0.0 || a > 1.0 || r > 1.0) {
            FragColor = vec4(vec3(0.0), 1.0);
            return;
        }
    }
    vec4 result = texture(sourceTexture, coord);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
