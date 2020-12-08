#version 440 core

uniform sampler2D sourceTexture;
uniform vec3 color = vec3(1.0);

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 tex = texture(sourceTexture, texCoords);
    vec3 c = tex.rgb*color;
    FragColor = vec4(c, tex.a);
}
