#version 440 core
uniform sampler2D sourceTexture;
uniform sampler2D maskTexture;
uniform float inputMin = 0.0;
uniform float inputMax = 1.0;
uniform float outputMin = 0.0;
uniform float outputMax = 1.0;
uniform bool useMask = false;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(sourceTexture, texCoords);
    vec4 outputValue = (texColor - vec4(inputMin))/vec4(inputMax - inputMin)*vec4(vec3(outputMax - outputMin), 1.0)
                       + vec4(vec3(outputMin), 0.0);
    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        outputValue *= mask;
    }
    FragColor = outputValue;
}
