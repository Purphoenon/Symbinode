#version 440 core

uniform sampler2D albedoTex;
uniform bool useAlbedoTex = false;
uniform vec3 albedoVal;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = useAlbedoTex ? texture(albedoTex, texCoords) : vec4(albedoVal, 1.0);
}
