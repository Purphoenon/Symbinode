#version 440 core
uniform samplerCube cubemap;

uniform float lod = 0.0;

out vec4 FragColor;
in vec2 texCoords;

void main()
{
    vec2 thetaphi;// = ((texCoords*2.0) - vec2(1.0)) * vec2(3.1415926535897932384626433832795, 1.5707963267948966192313216916398);
    thetaphi.x = (1.0 - texCoords.x)*6.28318 - 1.57079;
    thetaphi.y = texCoords.y*3.14159 - 1.57079;
    vec3 rayDirection = vec3(cos(thetaphi.y) * cos(thetaphi.x), sin(thetaphi.y), cos(thetaphi.y) * sin(thetaphi.x));
    FragColor = textureLod(cubemap, normalize(rayDirection), lod);
}
