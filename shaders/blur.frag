#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float intensity = 0.5;
uniform vec2 resolution;
uniform vec2 direction;
uniform bool useMask = false;
vec2 size = 0.0015*resolution*intensity;

in vec2 texCoords;

out vec4 FragColor;

float gauss (float x, float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    vec4 color = vec4(0.0);
    float total = 0.0;
    float g = gauss(0, 5.0);
    vec4 texColor = texture(sourceTexture, texCoords);
    color += texColor*g;
    total += g;
    for(int i = 1; i < 10; ++i) {
        float w = gauss(i, 5.0);
        total += 2*w;
        color += w * texture(sourceTexture, texCoords + (direction*i/resolution)*size);
        color += w * texture(sourceTexture, texCoords - (direction*i/resolution)*size);
    }

    color /= total;

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor  = color;
}
