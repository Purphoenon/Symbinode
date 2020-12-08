#version 440 core

out vec4 FragColor;

const float N = 20.0;
uniform float scale = 1.0;
uniform vec2 offset;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy + offset;
    float pW1 = 1.0;
    float pW2 = 2.0;

    float pix = 1.5;

    vec2 p1 = abs(mod(fragCoord + pW1/2.0, N*scale));
    vec2 p2 = abs(mod(fragCoord + pW2/2.0, 5.0 * N*scale));

    float g1 = min(p1.x, p1.y) + 1.0 - pW1;
    float g2 = min(p2.x, p2.y) + 1.0 - pW2;
    float m = clamp(min(g1, g2), 0.16, 1.0);
    if(m == 1.0) discard;
    //FragColor = vec4(0.25, 0.25, 0.25, 1.0);
    if(g2 < g1) {
        FragColor = vec4(0.23, 0.24, 0.25, 1.0);

    }
    else {
        FragColor = vec4(0.25, 0.27, 0.28, 1.0);
    }
    //if(m != 0.16) m = 0.18;

    //FragColor = vec4(vec3(m), 1.0);
}
