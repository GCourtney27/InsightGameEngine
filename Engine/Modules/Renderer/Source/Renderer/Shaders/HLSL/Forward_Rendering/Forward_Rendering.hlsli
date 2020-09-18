#include <../Common/Lights_Common.hlsli>

cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPerFrame : register(b1)
{
    float3 cameraPosition;
    float cameraExposure; //4
    float4x4 view;
    float4x4 inverseView;
    float4x4 projection;
    float cameraNearZ;
    float4x4 inverseProjection;
    float cameraFarZ;
    float DeltaMs;
    float time; //4
    float numPointLights;
    float numDirectionalLights;
    float numSpotLights;
    float padding; //4
    float2 screenSize;
    float padding1;
    float padding2;
};

cbuffer cbLights : register(b2)
{
    PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
    DirectionalLight dirLights[MAX_DIRECTIONAL_LIGHTS_SUPPORTED];
    SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};

cbuffer cbPerObjectAdditive : register(b3)
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