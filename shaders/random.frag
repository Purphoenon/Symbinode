#version 440 core

uniform int seed = 1;

out vec4 FragColor;

const float PHI = 1.61803398874989484820459;

float gold_noise(vec2 xy, int seed)
{
    return fract(tan(distance(xy*PHI, xy)*1367.454541*seed)*xy.x);
}

void main()
{
    FragColor = vec4(gold_noise(gl_FragCoord.xy, seed), gold_noise(gl_FragCoord.xy, seed + 30), gold_noise(gl_FragCoord.xy, seed + 3), gold_noise(gl_FragCoord.xy, seed + 34));
}
