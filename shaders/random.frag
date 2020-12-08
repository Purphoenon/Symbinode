#version 440 core

out vec4 FragColor;

const float PHI = 1.61803398874989484820459;

float gold_noise(vec2 xy, float seed)
{
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{
    FragColor = vec4(gold_noise(gl_FragCoord.xy, 56.0), gold_noise(gl_FragCoord.xy, 86.0), gold_noise(gl_FragCoord.xy, 3.0), gold_noise(gl_FragCoord.xy, 34.0));
}
