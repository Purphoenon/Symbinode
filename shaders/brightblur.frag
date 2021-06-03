#version 440 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D image;
uniform vec2 direction;
uniform vec2 resolution;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
     vec2 tex_offset = 1.0 / (resolution);
     vec3 result = texture(image, texCoords).rgb * weight[0];
     for(int i = 1; i < 5; ++i)
     {
         result += texture(image, (texCoords + tex_offset*i*direction)).rgb * weight[i];
         result += texture(image, (texCoords - tex_offset*i*direction)).rgb * weight[i];
     }
     FragColor = vec4(result, 1.0);
}
