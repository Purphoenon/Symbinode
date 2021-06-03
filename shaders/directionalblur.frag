#version 440 core

#define PI 3.14159265359

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float intensity = 2.0;
uniform vec2 resolution;
uniform int angle = 0;
uniform bool useMask = false;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}

void main()
{
    vec4 color = vec4(0.0);

    vec2 dir = rotate2d(PI*angle/180)*vec2(-1.0, 0.0);
    vec2 size = 0.001*resolution*intensity;
    color = texture2D(sourceTexture, vec2(gl_FragCoord) / resolution) * weight[0];
    for (int i=1; i<3; i++) {
        color +=
            texture2D(sourceTexture, (vec2(gl_FragCoord) + dir*offset[i]*size) / (resolution))
                * weight[i];
        color +=
            texture2D(sourceTexture, (vec2(gl_FragCoord) - dir*offset[i]*size) / (resolution))
                * weight[i];
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor  = color;
}
