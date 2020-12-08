#version 440 core

uniform sampler2D tex;
uniform bool useTex = false;
uniform float val;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = useTex ? vec4(vec3(texture(tex, texCoords).r), 1.0) : vec4(vec3(val), 1.0);
}
