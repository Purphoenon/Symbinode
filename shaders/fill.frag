#version 440 core

uniform sampler2D sourceTexture;

out vec4 FragColor;

vec2 offset[4] = vec2[](vec2(-1.0, 0.0),
                        vec2(0.0, 1.0),
                        vec2(0.0, -1.0),                        
                        vec2(1.0, 0.0));

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

    for(int o = 0; o < 4; ++o) {
        vec2 uv2 = uv;
        vec2 off = offset[o]/texSize;
        for(int i = 1; i < texSize.x; ++i) {
            uv2 += off;
            vec4 c2 = texture(sourceTexture, uv2);
            if(c2.z + c2.w == 0.0) break;
            vec2 startPoint = min(c.xy, c2.xy + floor(uv2));
            vec2 rect = max(c.xy + c.zw, c2.xy + floor(uv2) + c2.zw) - startPoint;
            c = vec4(startPoint, rect);
        }
    }
    c.xy = (uv - c.xy)/c.zw;
    if(c.z > 1.0) c.x = uv.x;
    if(c.w > 1.0) c.y = uv.y;

    FragColor = c;
}
