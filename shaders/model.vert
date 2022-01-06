#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 WorldPos;
out vec3 Normal;

void main()
{
    WorldPos = vec3(model*vec4(aPos, 1.0));
    Normal = normalize(mat3(model) * aNormal);
    gl_Position = projection*view*vec4(WorldPos, 1.0);
}
