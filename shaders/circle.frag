#version 440 core

uniform sampler2D maskTexture;
uniform vec2 res;
uniform float radius = 0.5;
uniform float smoothValue = 0.01;
uniform int interpolation = 1; //0 - linear, 1 - Hermite
uniform bool useMask = false;

out vec4 FragColor;

void main()
{
    vec2 st = gl_FragCoord.xy/res;
    float pct = 0.0;

    st = st*2.0 - 1.0;
    pct = distance(st,vec2(0.0));
    float edge0 = radius - smoothValue*radius;
    float edge1 = radius;
    float t = 0.0;
    if(interpolation == 0) {
        t = clamp((pct - edge0) / (edge1 - edge0), 0.0, 1.0);
    }
    else if(interpolation == 1) {
        t = smoothstep(edge0, edge1, pct);
    }
    float circle = 1.0 - t;

    vec4 color = vec4(circle);

    if(useMask) {
        vec4 maskColor = texture(maskTexture, gl_FragCoord.xy/res);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }

    FragColor = color;
}
