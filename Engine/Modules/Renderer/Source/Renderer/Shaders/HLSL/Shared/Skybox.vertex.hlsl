#include <../Deferred_Rendering/Deferred_Rendering.hlsli>	

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outTexCoord : TEXCOORD;
};

// Entry Point
// -----------
PS_OUTPUT main(VS_INPUT vs_in)
{
    PS_OUTPUT vs_out;
    
    matrix viewNoMovement = cbView;
    viewNoMovement._41 = 0;
    viewNoMovement._42 = 0;
    viewNoMovement._43 = 0;
    
    matrix viewProjection = mul(viewNoMovement, cbProjection);
    vs_out.outPosition = mul(float4(vs_in.inPosition, 1.0), viewProjection);
    vs_out.outPosition.z = vs_out.outPosition.w;
    
    vs_out.outTexCoord = vs_in.inPosition;
    
    return vs_out;
}
