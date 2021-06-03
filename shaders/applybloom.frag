#version 440 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure = 1.0f;
uniform float intensity = 1.0f;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, texCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    hdrColor += bloomColor*0.25*intensity;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}
