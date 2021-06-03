#version 440 core

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform sampler2D maskTexture;
uniform vec2 res;
uniform int columns = 5;
uniform int rows = 6;
uniform float hexSmooth = 0.0;
uniform float size = 0.9;
uniform float maskStrength = 0.0;
uniform int seed = 1;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

const float PHI = 1.61803398874989484820459;

float gold_noise(vec2 xy, int seed)
{
    return fract(tan(distance(xy*PHI, xy)*1367.454541*seed)*xy.x);
}

void main()
{
    vec2 st = gl_FragCoord.xy/res;
    vec3 color = vec3(0.0);

    st *= vec2(columns, rows);

    vec2 cell = floor(st);
    vec2 off = fract(st);

    int N = 6;

    for(int i = -1; i <= 1; ++i) {
        for(int j = -1; j <= 1; ++j) {
            vec2 cell_t = cell + vec2(i, j);
            cell_t = mod(cell_t, vec2(columns, rows));
            vec2 off_t = off - vec2(i, j);
            vec2 coords = (off_t - vec2(mod(cell_t.y, 2.0)*0.5, 0.0));
            coords -= 0.5;
            coords /= vec2(1.0, 1.16);
            coords += 0.5;
            coords = (2.0*coords)-1.0;
            float a = atan(coords.x,coords.y)+PI*0.5;
            float r = TWO_PI/float(N);

            float mask = 1.0 - gold_noise(cell_t + 1.0, seed)*maskStrength;

            float d = (cos(floor(.5+a/r)*r-a))*length(coords);

            float p = 1.0-smoothstep(size - hexSmooth, size, d);
            color = max(color, vec3(p)*mask);
        }
    }

    vec4 result = vec4(color, 1.0);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float maskValue = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= maskValue;
    }

    FragColor = result;
}
