#version 440 core

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform sampler2D maskTexture;
uniform vec2 res;
uniform int sides = 3;
uniform float scale = 0.4;
uniform float smoothValue = 0.0;
uniform bool useMask = false;

out vec4 FragColor;

void main() {
    vec2 st = gl_FragCoord.xy/res;
    vec4 color = vec4(0.0);
    float d = 0.0;

    st = (2.0*st)-1.0;

    int N = sides;

    float a = atan(st.x,st.y)+PI;
    float r = TWO_PI/float(N);

    d = cos(floor(.5+a/r)*r-a)*length(st);

    float p = 1.0-smoothstep(scale - smoothValue*scale, scale,d);
    color = vec4(p);

    if(useMask) {
        vec4 maskColor = texture(maskTexture, gl_FragCoord.xy/res);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor = color;
}
