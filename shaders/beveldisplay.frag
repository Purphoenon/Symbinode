#version 440 core

uniform sampler2D dfTexture;
uniform sampler2D maskTexture;
uniform float dist = 0.05;
uniform bool useAlpha = false;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{    
    vec2 texSize = textureSize(dfTexture, 0);

    vec2 uv = gl_FragCoord.xy/texSize;

    float div = texSize.x*abs(dist);
    div = clamp(div, 1.0, texSize.x);
    float d = (length(texture(dfTexture, uv).xy*texSize - gl_FragCoord.xy))/div;
    d = max(0.0, min(1.0, d));
    d = (d - 0.5)*2.0 + 0.5;
    d += 0.5;
    if(dist > 0.0) d = 1.0 - d;

    float alpha = 1.0;
    if(useAlpha) alpha = d;

    vec4 result = vec4(vec3(d), alpha);

    if(useMask) {
        vec4 maskColor = texture(maskTexture, uv);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }

    FragColor = result;
}
