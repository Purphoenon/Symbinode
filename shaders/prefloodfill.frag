#version 440 core

uniform vec2 res;
uniform sampler2D grayscaleTexture;

vec2 offset[4] = vec2[](vec2(-1.0, 0.0),
                        vec2(0.0, -1.0),
                        vec2(0.0, 1.0),
                        vec2(1.0, 0.0));

out vec4 FragColor;

void main() {
    vec2 st = gl_FragCoord.xy/res;
    vec2 texSize = textureSize(grayscaleTexture, 0);

    float lum = dot(texture(grayscaleTexture, st).rgb, vec3(0.299, 0.587, 0.114));
    if(lum > 0.5) {
        float mask = lum;
        for(int o = 0; o < 4; ++o) {
            vec4 maskColor = texture(grayscaleTexture, st - offset[o]/texSize);
            mask = min(mask, dot(maskColor.rgb, vec3(0.299, 0.587, 0.114)));
            if(mask == 0.0) {
                break;
            }
        }
        mask = mask < 1.0 ? 0.0 : 1.0;
        FragColor = vec4(st, 1.0/256.0, 1.0/256.0)*mask;
    }
    else {
        FragColor = vec4(0.0);
    }
}
