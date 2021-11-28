#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float radius = 2.0;
uniform vec2 resolution;
uniform vec2 direction;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

float gauss (float x, float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    vec4 color = vec4(0.0);
    float sum = 0.0;
    float k = resolution.x/1024.0;
    float sigma = max(0.001, k*radius);
    float g = gauss(0.0, sigma);
    color = texture(sourceTexture, vec2(gl_FragCoord) / resolution)*g;
    sum += g;

    for (int i=1; i<ceil(k*radius*3.0); i++) {
        g = gauss(i, sigma);
        color += texture(sourceTexture, (gl_FragCoord.xy + i*direction) / resolution )*g;
        color += texture(sourceTexture, (gl_FragCoord.xy - i*direction) / resolution )*g;
        sum += g + g;
    }

    color /= sum;

    float alpha = color.a;
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        alpha = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
    }

    FragColor = vec4(color.rgb, alpha);
}
