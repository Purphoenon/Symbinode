#version 440 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 TexCoords;

uniform mat4 matrix;

out vec2 texCoords;

void main()
{
    texCoords = TexCoords;
    gl_Position = matrix * vec4(pos, 0.0, 1.0);
}
