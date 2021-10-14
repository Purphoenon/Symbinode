#version 440 core

uniform sampler2D textureSample;
uniform sampler2D maskTexture;

in vec2 texCoords;

out vec4 FragColor;

vec2 offset[4] = vec2[](vec2(-1.0, 0.0),
                        vec2(0.0, -1.0),
                        vec2(0.0, 1.0),
                        vec2(1.0, 0.0));

void main()
{
    vec4 result = texture(textureSample, texCoords);
    vec2 texSize = textureSize(maskTexture, 0);

    vec4 maskColor = texture(maskTexture, texCoords);
    float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);

    //dilation
    if(mask > 0.5 && result.a == 0.0) {
        for(int i = 0; i < 4; ++i) {
            vec4 c = texture(textureSample, texCoords + offset[i]/texSize);
            result = max(result, c);
        }
    }
    result *= step(0.5, mask);

    FragColor = result;
}
