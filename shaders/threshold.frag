#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float threshold;
uniform bool useMask = false;

in vec2 texCoords;
out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    float b = 0.33333*(texColor.r + texColor.g + texColor.b);
    float c = step(threshold, b);
    vec4 result = vec4(vec3(c), step(threshold, texColor.a));
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
