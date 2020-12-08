#version 440 core

uniform sampler2D sourceTexture;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    vec3 color = vec3(1.0) - texColor.rgb;
    FragColor = vec4(color, texColor.a);
}
