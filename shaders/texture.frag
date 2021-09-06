#version 440 core

uniform sampler2D textureSample;
uniform float lod = 0.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = textureLod(textureSample, texCoords, lod);
}
