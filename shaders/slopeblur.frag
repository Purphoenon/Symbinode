#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D slopeTexture;
uniform sampler2D maskTexture;
uniform float intensity = 0.1f;
uniform int samples = 1;
uniform int mode = 0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec2 step = vec2(0.001);
    vec2 uv = texCoords;

    float tl = texture(slopeTexture, uv + vec2(-step.x, step.y)).x;
    float t = texture(slopeTexture, uv + vec2(0.0, step.y)).x;
    float tr = texture(slopeTexture, uv + vec2(step.x, step.y)).x;
    float bl = texture(slopeTexture, uv + vec2(-step.x, -step.y)).x;
    float b = texture(slopeTexture, uv + vec2(0.0, -step.y)).x;
    float br = texture(slopeTexture, uv + vec2(step.x, -step.y)).x;
    float l = texture(slopeTexture, uv + vec2(-step.x, 0.0)).x;
    float r = texture(slopeTexture, uv + vec2(step.x, 0.0)).x;

    float top_side = tl + 2.0*t + tr;
    float bottom_side = bl + 2.0*b + br;
    float right_side = tr + 2.0*r + br;
    float left_side = tl + 2.0*l + bl;
    vec2 n = vec2(right_side - left_side, top_side - bottom_side);

    float offset = intensity/samples;
    float f = offset;
    vec4 result = texture(sourceTexture, uv + f*n);
    vec4 s = vec4(0.0);
    if(mode == 0) { //average
        for(int i = 1; i < samples; ++i) {
            f += offset;
            s = texture(sourceTexture, uv + f*n);
            result += s;
        }
    }
    else if(mode == 1) { //min
        for(int i = 1; i < samples; ++i) {
            f += offset;
            s = texture(sourceTexture, uv + f*n);
            result += min(s, result);
        }
    }
    else if(mode == 2) { //max
        for(int i = 1; i < samples; ++i) {
            f += offset;
            s = texture(sourceTexture, uv + f*n);
            result += max(s, result);
        }
    }
    result /= samples;
    if(useMask) {
        vec4 maskColor = texture(maskTexture, uv);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
