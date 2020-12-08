#version 440 core

uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform int dir = 0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec2 coords = texCoords;
    if(dir == 0) { //left to right
        coords.x = coords.x < 0.5 ? coords.x : 1.0 - coords.x;
    }
    else if(dir == 1) { //right to left
        coords.x = coords.x > 0.5 ? coords.x : 1.0 - coords.x;
    }
    else if(dir == 2) { //top to bottom
        coords.y = coords.y > 0.5 ? coords.y : 1.0 - coords.y;
    }
    else if(dir == 3) { //bottom to top
        coords.y = coords.y < 0.5 ? coords.y : 1.0 - coords.y;
    }
    vec4 result = texture(sourceTexture, coords);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        result *= mask;
    }
    FragColor = result;
}
