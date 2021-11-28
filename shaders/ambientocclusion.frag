#version 440 core
uniform sampler2D heightTexture;
uniform sampler2D noiseTexture;

uniform float radius = 0.1;
uniform int samples = 8;
uniform float strength = 0.3;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    float z = 1.0 - texture(heightTexture, texCoords).r;
    float ao = 0.0;
    vec2 noiseScale = textureSize(heightTexture, 0)/textureSize(noiseTexture, 0);
    for(int i = 0; i < samples; ++i) {
        vec2 off = 2.0*texture(noiseTexture, (texCoords*textureSize(heightTexture, 0) + 28.97*i)/textureSize(noiseTexture, 0)).xy - 1.0;
        float sz = 1.0 - texture(heightTexture, texCoords + 0.3*radius*off).r;
        ao += smoothstep(0.0, 1.0, (z - sz));
    }

    ao = clamp(1.0 - ao/samples*strength*10.0, 0.0, 1.0);
    vec3 color = vec3(ao);
    FragColor = vec4(color, 1.0);
}
