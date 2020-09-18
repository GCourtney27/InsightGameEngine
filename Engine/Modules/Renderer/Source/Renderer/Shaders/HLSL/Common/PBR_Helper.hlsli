// This header includes all functions necessary 
// for Physically Based Rendering in the Pixel Shader 

static const float PI = 3.14159265359;

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float demon = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    demon = PI * demon * demon;
    return a2 / max(demon, 0.0000001); // Prevent divide by 0
}
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    float ggx1 = NdotV / (NdotV * (1.0f - k) + k); // Schlick GGX
    float ggx2 = NdotL / (NdotL * (1.0f - k) + k);
    return ggx1 * ggx2;
}
float3 FresnelSchlick(float HdotV, float3 baseReflectivity)
{
	// Base reflectivity in range from 0 to 1
	// returns range of base reflectivity to 1
	// inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
    return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - HdotV, 5.0f);
}
float3 FresnelSchlickRoughness(float HdotV, float3 F0, float roughness)
{
	// Base reflectivity in range from 0 to 1
	// returns range of base reflectivity to 1
	// inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - HdotV, 5.0f);
}
