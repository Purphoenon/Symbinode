#version 440 core

uniform sampler2D normalTexture;
uniform sampler2D maskTexture;

uniform bool useMask = false;
uniform float intensity = 1.0;
uniform float offset = 1.0;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 t = texture(normalTexture, texCoords + vec2(0.0, offset)/1024.0);
    vec4 b = texture(normalTexture, texCoords + vec2(0.0, -offset)/1024.0);
    vec4 r = texture(normalTexture, texCoords + vec2(offset, 0.0)/1024.0);
    vec4 l = texture(normalTexture, texCoords + vec2(-offset, 0.0)/1024.0);
    //emboss filter
    float red = intensity*(r.r - l.r) + 0.5;
    float green = intensity*(b.g - t.g) + 0.5;
    //overlay blend
    float c = 0.0;
    if(2.0*green <= 1.0) {
        c = 2.0*red*green;
    }
    else {
        c = 1.0 - 2.0*(1.0 - green)*(1.0 - red);
    }

    float alpha = 1.0;
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        alpha = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
    }

    FragColor = vec4(vec3(c), alpha);
}
