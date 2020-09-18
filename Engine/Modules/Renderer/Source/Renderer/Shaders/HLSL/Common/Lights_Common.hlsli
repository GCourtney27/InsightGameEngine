
/*
    This file includes all lights that are able to be rendered inside a pixel shader.
    * Note: Constant Buffers must be 16 byte aligned, edit with caution.
*/

// If max supported lights is changed here it must also be changed inside <Insight/Core.h>
#define MAX_POINT_LIGHTS_SUPPORTED 16
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 4
#define MAX_SPOT_LIGHTS_SUPPORTED 16

#include <PBR_Helper.hlsli>

struct PointLight
{
    float3 position;
    float strength;
	
    float3 diffuse;
    float padding;
};

struct DirectionalLight
{
    float3 direction;
    float padding;
    
    float3 diffuse;
    float strength;
    
    float4x4 lightSpaceView;
    float4x4 lightSpaceProj;
};

struct SpotLight
{
    float3 position;
    float innerCutOff;
    float3 direction;
    float outerCutOff;

    float3 diffuse;
    float strength;
};

float3 CaclualteDirectionalLight(DirectionalLight Light, float3 ViewDirection, float3 WorldNormal, float3 WorldPosition, float NdotV, float3 MaterialAlbedo, float MaterialRoughness, float MaterialMetallic, float3 BaseReflectivity)
{
    float3 LightDir = normalize(-Light.direction);
    float3 HalfwayDir = normalize(ViewDirection + LightDir);
    float3 Radiance = (Light.diffuse) * Light.strength;
        
    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);
        
    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);
        
    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;
        
    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;
        
    return ((kD * MaterialAlbedo / PI + Specular) * Radiance * NdotL);
}

float3 CalculateSpotLight(SpotLight Light, float3 ViewDirection, float NdotV, float3 WorldPosition, float3 WorldNormal, float3 MaterialAlbedo, float MaterialRoughness, float MaterialMetallic, float3 BaseReflectivity)
{
    float3 LightDir = normalize(Light.position - WorldPosition);
    float3 HalfwayDir = normalize(LightDir + ViewDirection);
    float Distance = length(Light.position - WorldPosition);
    float Attenuation = 1.0 / (Distance * Distance);
        
    float Theta = dot(LightDir, normalize(-Light.direction));
    float Epsilon = Light.innerCutOff - Light.outerCutOff;
    float Intensity = clamp((Theta - Light.outerCutOff) / Epsilon, 0.0, 1.0);

    float3 radiance = ((Light.diffuse * (Light.strength * 10000.0)) * Intensity) * Attenuation;
        
    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);
        
    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);
        
    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;
        
    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;
    
    return (kD * MaterialAlbedo / PI + Specular) * radiance * NdotL;
}

float3 CalculatePointLight(PointLight Light, float3 WorldPosition, float3 ViewDirection, float NdotV, float3 WorldNormal, float3 MaterialAlbedo, float MaterialMetallic, float MaterialRoughness, float3 BaseReflectivity)
{
    float3 LightDir = normalize(Light.position - WorldPosition);
    float3 HalfwayDir = normalize(ViewDirection + LightDir);
    float Distance = length(Light.position - WorldPosition);
    float Attenuation = 1.0 / (Distance * Distance);
    float3 Radiance = ((Light.diffuse * 255.0) * Light.strength) * Attenuation;
        
    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);
        
    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);
        
    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;
        
    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;
    
    return (kD * MaterialAlbedo / PI + Specular) * Radiance * NdotL;
}
