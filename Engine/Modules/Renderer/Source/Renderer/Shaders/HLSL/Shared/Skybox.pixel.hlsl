#include <../Common/Insight_Common.hlsli>

cbuffer cbPerFrame : register(b1)
{
    float3 cbCameraPosition;
    float cbCameraExposure; //4
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

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 texCoords : TEXCOORD;
};

TextureCube t_SkyMap : register(t14);

SamplerState s_LinearWrapSampler : register(s1);

RWTexture2D<float4> rw_FinalImage : register(u1);


float3 main(PS_INPUT ps_in) : SV_TARGET
{
    float3 diffuse = t_SkyMap.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    
    float2 uvs; // = GetPixelCoords(ps_in.inPosition.xy, cbScreenSize);
    uvs.x = (ps_in.inPosition.x + 1) * cbScreenSize.x * 0.5 + 0;
    uvs.y = (1 - ps_in.inPosition.y) * cbScreenSize.y * 0.5 + 0;
    //rw_FinalImage[uvs] = float4(diffuse, 1.0);
    
    return diffuse;
}