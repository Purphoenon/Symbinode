#version 440 core

uniform sampler2D sourceTexture;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sourceTexture, texCoords);
    float gray = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(vec3(gray), color.a);
}
