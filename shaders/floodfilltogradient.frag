#version 440 core
#define PI 3.14159265359

uniform sampler2D floodFillTexture;
uniform sampler2D randomTexture;
uniform int rotation = 0;
uniform float randomizing = 1.0;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main()
{
    vec4 ff = texture(floodFillTexture, texCoords);
    if(ff.w == 0.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    vec2 uv = ff.xy;
    vec2 center = texCoords - uv*ff.zw + 0.5*ff.zw;
    uv -= 0.5;
    uv *= rotate2d(rotation*PI/180.0 + randomizing*texture(randomTexture, center).z*PI);
    uv += 0.5;
    vec2 a = uv;
    vec2 b = vec2(1.0, 0.0);
    float cosA = dot(a, b)/(length(a)*length(b));
    float d = cosA*length(a);
    float f = d/length(b);
    float color = mix(1.0, 0.0, f);
    FragColor = vec4(vec3(color), 1.0);
}
