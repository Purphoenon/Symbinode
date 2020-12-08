#version 440 core
uniform sampler2D sourceTexture;
uniform float brightness = 0.5;
uniform float contrast = 0.5;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    vec3 color = texColor.rgb/texColor.a;
    color = (color - 0.5)*(contrast + 1.0) + 0.5;
    color += brightness;
    color *= texColor.a;
    FragColor = vec4(color, texColor.a);
}
