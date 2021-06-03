#version 440 core

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform sampler2D maskTexture;
uniform vec2 res;
uniform float scale = 1.0;
uniform int columns = 5;
uniform int rows = 15;
uniform float offset = 0.5;
uniform float width = 0.9;
uniform float height = 0.8;
uniform float smoothX = 0.0;
uniform float smoothY = 0.0;
uniform float maskStrength = 0.0;
uniform int seed = 1;
uniform bool useMask;

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
    st.x += step(1.0, mod(st.y, 2.0))*offset;
    vec2 maskUV = st;
    maskUV.x = mod(maskUV.x, columns);
    float mask = 1.0 - gold_noise(floor(maskUV + 1.0), seed)*maskStrength;
    st = fract(st);

    st -= 0.5;
    st /= vec2(width, height);
    st += 0.5;

    st = (2.0*st)-1.0;

    int N = 4;

    float r = TWO_PI/float(N);

    float aX = atan(st.x,0.0)+PI;
    float dX = (cos(floor(.5+aX/r)*r-aX))*length(vec2(st.x, 0.0));
    float aY = atan(0.0,st.y)+PI;
    float dY = (cos(floor(.5+aY/r)*r-aY))*length(vec2(0.0, st.y));

    float pX = 1.0-smoothstep(scale - smoothX, scale, dX);
    float pY = 1.0-smoothstep(scale - smoothY, scale, dY);
    float p = min(pX, pY);
    color = vec3(p);
    color *= mask;

    vec4 result = vec4(color, 1.0);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float maskValue = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= maskValue;
    }

    FragColor = result;
}
