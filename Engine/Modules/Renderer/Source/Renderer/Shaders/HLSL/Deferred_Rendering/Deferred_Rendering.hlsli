#include <../Common/Lights_Common.hlsli>

#define MAX_PER_OBJECT_LOD 9

cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPerFrame : register(b1)
{
    float3 cbCameraPosition;
    float cbCameraExposure;//4
    float4x4 cbView;
    float4x4 cbInverseView;
    float4x4 cbProjection;
    float4x4 cbInverseProjection;
    float cbCameraNearZ;
    float cbCameraFarZ;
    float cbDeltaMs;
    float cbTime; //4
    float cbNumPointLights;
    float cbNumDirectionalLights;
    float cbNumSpotLights;
    float cbRayTraceEnabled; //4
    float2 cbScreenSize;
	float padding1;
	float padding2;
};

cbuffer cbLights : register(b2)
{
    PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};

cbuffer cbPostFx : register(b3)
{
    // Vignette
    float vnInnerRadius;
    float vnOuterRadius;
    float vnOpacity;
    int vnEnabled;
    
	// Film Grain
    float fgStrength;
    int fgEnabled;
    
    // Chromatic Aberration
    int caEnabled;
    float caIntensity;
}

cbuffer cbPerObjectAdditive : register(b4)
{
    float roughnessAdditive;
    float metallicAdditive;
    float2 uvOffset;
    
    float2 tiling;
    float padding3;
    float padding4;
    
    float3 diffuseAdditive;
    float padding5;
};

/* Shadow Pass */
struct VS_INPUT_SHADOWPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VS_OUTPUT_SHADOWPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_INPUT_SHADOWPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

/* Geometry Pass */
struct VS_INPUT_GEOMPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VS_OUTPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_INPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

/* Lighting Pass */
struct VS_INPUT_LIGHTPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
};

struct VS_OUTPUT_LIGHTPASS
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

struct PS_INPUT_LIGHTPASS
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

