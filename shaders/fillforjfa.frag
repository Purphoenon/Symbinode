#version 440 core

uniform sampler2D sourceTexture;
uniform bool outer = false;

out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy/(textureSize(sourceTexture, 0));
    float lum = dot(texture(sourceTexture, uv).rgb, vec3(0.299, 0.587, 0.114));
    if((lum > 0.5 && outer) || (lum < 0.5 && !outer)) {
        FragColor = vec4(uv, 0.0, 1.0);
    }
    else {
        FragColor = vec4(0.0);
    }
}
