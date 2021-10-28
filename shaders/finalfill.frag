#version 440 core

uniform sampler2D sourceTexture;
uniform vec2 dir;

out vec4 FragColor;

void main()
{
    vec2 texSize = textureSize(sourceTexture, 0);
    vec2 uv = gl_FragCoord.xy/texSize;
    vec4 color = vec4(0.0);

    vec4 c = texture(sourceTexture, uv);
    if(c.z + c.w == 0.0) {
        FragColor = vec4(0.0);
        return;
    }

    vec2 uv2 = uv;
    vec2 uv3 = uv;
    vec2 off = dir/texSize;

    vec4 c2 = c;
    vec4 c3 = c;

    for(int i = 1; i < 128; ++i) {
        if(c2.z + c2.w > 0.0) {
            uv2 += off;
            c2 = texture(sourceTexture, uv2);
            vec2 globalPos = uv - c.zw*c.xy;
            vec2 globalPos2 = uv2 - c2.zw*c2.xy;
            vec2 startPoint = vec2(min(globalPos.x, globalPos2.x), min(globalPos.y, globalPos2.y));
            vec2 rect = vec2(max(globalPos.x + c.z, globalPos2.x + c2.z), max(globalPos.y + c.w, globalPos2.y + c2.w)) - startPoint;
            c = vec4(startPoint, rect);
            c.xy = (uv - c.xy)/c.zw;
            if(c.z >= 1.0) c.x = uv.x;
            if(c.w >= 1.0) c.y = uv.y;
        }
        if(c3.z + c3.w > 0.0) {
            uv3 -= off;
            c3 = texture(sourceTexture, uv3);
            vec2 globalPos = uv - c.zw*c.xy;
            vec2 globalPos3 = uv3 - c3.zw*c3.xy;
            vec2 startPoint = vec2(min(globalPos.x, globalPos3.x), min(globalPos.y, globalPos3.y));
            vec2 rect = vec2(max(globalPos.x + c.z, globalPos3.x + c3.z), max(globalPos.y + c.w, globalPos3.y + c3.w)) - startPoint;
            c = vec4(startPoint, rect);

            c.xy = (uv - c.xy)/c.zw;
            if(c.z >= 1.0) c.x = uv.x;
            if(c.w >= 1.0) c.y = uv.y;
        }
        if(c2.z + c2.w + c3.z + c3.w == 0.0) break;
    }

    FragColor = c;
}
