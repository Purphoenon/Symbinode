#version 440 core

uniform sampler2D textureSample;
uniform float lod = 0.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    /*vec3 color = textureLod(textureSample, texCoords, lod).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));*/
    FragColor = textureLod(textureSample, texCoords, lod);//vec4(color, 1.0);
}
