#version 440 core

uniform sampler2D textureSample;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = texture(textureSample, texCoords);
}
