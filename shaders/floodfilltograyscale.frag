#version 440 core

uniform sampler2D floodFillTexture;
uniform sampler2D randomTexture;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 ff = texture(floodFillTexture, texCoords);
    if(ff.w == 0.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    vec2 uv = ff.xy;
    vec2 center = texCoords - uv*ff.zw + 0.5*ff.zw;
    float color = texture(randomTexture, center).z;
    FragColor = vec4(vec3(color), 1.0);
}
