#version 440 core

uniform vec2 res;
uniform sampler2D grayscaleTexture;
uniform float strength;

vec2 size = 0.001*res;
out vec4 FragColor;

void main() {
    vec2 st = gl_FragCoord.xy/res;
    st.x *= res.x/res.y;
    vec2 offset = vec2(1.0)/res*size;
    vec2 offsets[9] = vec2[](
         vec2(-offset.x,  offset.y), // top-left
         vec2( 0.0f,    offset.y), // top-center
         vec2( offset.x,  offset.y), // top-right
         vec2(-offset.x,  0.0f),   // center-left
         vec2( 0.0f,    0.0f),   // center-center
         vec2( offset.x,  0.0f),   // center-right
         vec2(-offset.x, -offset.y), // bottom-left
         vec2( 0.0f,   -offset.y), // bottom-center
         vec2( offset.x, -offset.y)  // bottom-right
     );
    float sobelX[9] = float[](-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1);
    float sobelY[9] = float[](-1, -2, -1,
                               0,  0,  0,
                               1,  2,  1);

    float dx = 0.0;
    float dy = 0.0;

    for(int i = 0; i < 9; ++i) {
        dx += sobelX[i] * texture(grayscaleTexture, st + offsets[i]).r;
        dy += sobelY[i] * texture(grayscaleTexture, st + offsets[i]).r;
    }
    float s = strength == 0 ? 0.05 : strength;
    vec3 norm = normalize(vec3(dx, dy, 1.0/abs(s))) * 0.5 + 0.5;
    if(strength < 0) {
       norm.y = 1.0 - norm.y;
    }
    FragColor = vec4(norm, 1.0);
}
