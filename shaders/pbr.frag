/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Pos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 tangentView;
in vec3 tangentFragPos;

uniform bool useAlbMap = false;
uniform bool useMetalMap = false;
uniform bool useRoughMap = false;
uniform bool useAOMap = false;
uniform bool useNormMap = false;
uniform bool useHeightMap = false;
uniform bool useEmisMap = false;

uniform int tilesSize = 1;

uniform float heightScale = 0.1;
uniform float emissiveStrenght = 10.0;
uniform bool bloom = false;

uniform vec3 albedoVal = vec3(0.8, 0.8, 0.0);
uniform float metallicVal;
uniform float roughnessVal;
uniform float aoVal;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D heightMap;
uniform sampler2D emissionMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

in vec3 camPos;
uniform vec3 lightPosition = vec3(-5, 5, 19);
uniform vec3 lightColor = vec3(300, 300, 300);

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
vec3 getNormalFromMap(vec2 coords)
{
    vec3 tangentNormal = texture(normalMap, coords).xyz*2.0 - vec3(1.0);

    mat3 TBN = mat3(Tangent, -Bitangent, Normal);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float numLayers = 128;

    float layerDepth = 1.0 / numLayers;

    float currentLayerDepth = 0.0;

    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = 1.0 - texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    cosTheta = min(cosTheta, 1);
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    cosTheta = min(cosTheta, 1.0);
    return F0 + (vec3(0.05) - F0 ) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
    vec3 V = normalize(camPos - WorldPos);

    vec2 coords = useHeightMap ? ParallaxMapping(TexCoords*tilesSize, normalize(tangentView - tangentFragPos)) : (TexCoords*tilesSize);

    vec4 texAlbedo = vec4(albedoVal, 1.0);
    vec4 texColor = texture(albedoMap, coords);
    if(useAlbMap) texAlbedo = texColor;

    vec3 albedo = texAlbedo.rgb;
    albedo = pow(albedo, vec3(2.2));
    float metallic = useMetalMap ? texture(metallicMap, coords).r : metallicVal;
    vec3 emissive = useEmisMap ? texture(emissionMap, coords).rgb*emissiveStrenght : vec3(0.0);

    float roughness = useRoughMap ? texture(roughnessMap, coords).r : roughnessVal;
    roughness = clamp(roughness, 0.05, 1.0);

    vec3 N = useNormMap ? getNormalFromMap(coords) : normalize(Normal);

    vec3 R = 2.0*max(dot(N, V), 0.0)*N - V;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    //-- point light
    float selfShadow = 1.0;
    vec3 L = normalize(lightPosition - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 Fp    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator    = NDF * G * Fp;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specularp = nominator / denominator;

    vec3 kSp = Fp;
    vec3 kDp = vec3(1.0) - kSp;
    kDp *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    Lo += (kDp * albedo / PI + specularp) * radiance * NdotL * selfShadow;
    //--

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F0 * brdf.x + brdf.y);

    vec3 ambient = (kD*diffuse + specular);

    vec3 color = (ambient + Lo + emissive);

    if(!bloom) {
        color = vec3(1.0) - exp(-color);
        color = pow(color, vec3(1.0/2.2));
    }

    FragColor = vec4(color , 1.0);
}
