#version 440 core

uniform sampler2D textureSample;
uniform sampler2D maskTexture;

uniform vec2 res;
uniform bool useMask = false;
uniform float transX = 0.0;
uniform float transY = 0.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec2 texSize = textureSize(textureSample, 0);
    vec2 k = res/texSize;
    vec2 translate = vec2(transX, transY)*max(res, texSize)/texSize;
    vec2 coords = k*(texCoords) + translate;
    if(coords.x > 0.0 && coords.x < 1.0 && coords.y > 0.0 && coords.y < 1.0) {
        float alpha = 1.0;
        if(useMask) {
            vec4 maskColor = texture(maskTexture, texCoords);
            alpha = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        }
        vec4 color = texture(textureSample, coords);
        //color.a *= alpha;
        FragColor = color;
    }
    else {
        FragColor = vec4(0.0);
    }
}
