#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D blurTexture;
uniform sampler2D maskTexture;

uniform bool useMask = false;
uniform float contrast = 0.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 srcColor = texture(sourceTexture, texCoords);
    vec4 blurColor = texture(blurTexture, texCoords);
    vec3 color = srcColor.rgb - blurColor.rgb + 0.5;
    color = (color - 0.5)*(contrast + 1.0) + 0.5;
    float alpha = srcColor.a;
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        alpha = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
    }
    FragColor = vec4(color, alpha);
}
