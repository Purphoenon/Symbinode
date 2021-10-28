#version 440 core

uniform vec2 res;
uniform sampler2D grayscaleTexture;

out vec4 FragColor;

void main() {
    vec2 st = gl_FragCoord.xy/res;
    vec2 texSize = textureSize(grayscaleTexture, 0);

    float lum = dot(texture(grayscaleTexture, st).rgb, vec3(0.299, 0.587, 0.114));
    if(lum > 0.5) {
        FragColor = vec4(st, 1.0/256.0, 1.0/256.0);
    }
    else {
        FragColor = vec4(0.0);
    }
}
