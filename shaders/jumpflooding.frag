#version 440 core

uniform sampler2D sourceTexture;
uniform int step;

out vec4 FragColor;

void main()
{
    vec2 texSize = textureSize(sourceTexture, 0);
    float dist = 99999.0;
    vec2 coord = vec2(0.0);
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 tc = texture(sourceTexture, (gl_FragCoord.xy + vec2(x, y)*step)/texSize).xy;
            if(tc.x != 0.0 && tc.y != 0.0) {
                vec2 diff = tc*texSize - (gl_FragCoord.xy);
                float d = length(diff);
                if(d < dist) {
                    dist = d;
                    coord = tc;
                }
            }
        }
    }
    FragColor = vec4(coord, 0.0, 1.0);
}
