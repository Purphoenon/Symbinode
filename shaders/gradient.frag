#version 440 core

const float TWO_PI = 6.28318530718;

subroutine float gradientType(vec2 st);
subroutine uniform gradientType gradientFunction;
uniform sampler2D maskTexture;
uniform vec2 res;
uniform vec2 startPos;
uniform vec2 endPos;
uniform float whiteWidth = 0.2;
uniform bool tiling = false;
uniform bool useMask = false;

out vec4 FragColor;

subroutine(gradientType)
float linear(vec2 st) {
    vec2 a = st - startPos;
    vec2 b = endPos - startPos;
    float cosA = dot(a, b)/(length(a)*length(b));
    float d = cosA*length(a);
    float f = tiling ? mod(d, length(b))/length(b) : d/length(b);
    float color = mix(1.0, 0.0, f);
    if(tiling) {
        //antialiasing
        float numPixels = 4.0;
        float aa = (1.0/res.x)*numPixels;
        float lerpLimit = max(1.0 - aa / length(b), 0.0);
        float invDiff = lerpLimit / (1.0-lerpLimit);
        if(color > lerpLimit)
            color = invDiff - color*invDiff;
    }
    return color;
}

subroutine(gradientType)
float reflected(vec2 st) {
    vec2 a = st - startPos;
    vec2 b = endPos - startPos;
    float cosA = dot(a, b)/(length(a)*length(b));
    float d = cosA*length(a);
    float f = tiling ? mod(d, length(b))/length(b) : d/length(b);
    float white1 = 0.5*(1.0 - whiteWidth);
    float white2 = 0.5*(1.0 + whiteWidth);
    float color = 0.0;
    if(f >= 0.0 && f < white1) {
        color = mix(0.0, 1.0, f/white1);
    }
    else if(f >= white1 && f <= white2) {
        color = 1.0;
    }
    else if(f > white2 && f < 1.0) {
        color = mix(1.0, 0.0, (f - white2)/(1.0 - white2));
    }
    return color;
}

subroutine(gradientType)
float angular(vec2 st) {
    vec2 a = st - startPos;
    vec2 b = endPos - startPos;
    float angleB = atan(b.y, b.x);
    float angleA = atan(a.y, a.x);
    float color = mod(angleA - angleB, TWO_PI)/TWO_PI;
    //antialiasing
    float numPixels = 4.0;
    float aa = (1.0/res.x)*numPixels;
    float lerpLimit = max(1.0 - aa / (TWO_PI*length(a)), 0.0);
    float invDiff = lerpLimit / (1.0-lerpLimit);
    if(color > lerpLimit)
        color = invDiff - color*invDiff;
    return color;
}

subroutine(gradientType)
float radial(vec2 st) {
    vec2 a = st - startPos;
    vec2 b = endPos - startPos;
    float f = length(a)/length(b);
    float color = mix(1.0, 0.0, f);
    return color;
}

void main()
{
    vec2 st = gl_FragCoord.xy/res;
    float color = gradientFunction(st);
    vec4 result = vec4(vec3(color), 1.0);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, st);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
