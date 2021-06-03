#version 440 core

uniform sampler2D textureSample;
uniform float threshold = 1.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec3 color = texture(textureSample, texCoords).rgb;
    float brightness = max(max(color.r, color.g), color.b);
    if(brightness > threshold) {
        FragColor = vec4(color, 1.0);
    }
    else
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
