#include <../Common/Insight_Common.hlsli>
#include <..//Deferred_Rendering/Deferred_Rendering.hlsli>

// Texture Inputs
// --------------
Texture2D t_AlbedoGBuffer : register(t0);
Texture2D t_NormalGBuffer : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer : register(t2);
Texture2D t_PositionGBuffer : register(t3);
Texture2D t_SceneDepthGBuffer : register(t4);

Texture2D t_ShadowDepthPass : register(t10);

Texture2D t_BloomPassResult : register(t15);

RWTexture2D<float4> rw_FinalImage : register(u1);

// Samplers
// --------
sampler s_PointClampSampler : register(s0);
SamplerState s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
float3 AddFilmGrain(float3 sourceColor, float2 texCoords);
float3 AddVignette(float3 sourceColor, float2 texCoords);
float3 AddChromaticAberration(float3 sourceColor, float2 texCoords);
float LinearizeDepth(float depth);

// Pixel Shader Return Value
// -------------------------
struct PS_INPUT_POSTFX
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

void main(PS_INPUT_POSTFX ps_in)
{
    float2 uvs = GetPixelCoords(ps_in.texCoords, cbScreenSize);
    float3 result = rw_FinalImage.Load(int3(uvs, 0.0)).rgb;
    
    if (vnEnabled)
    {
        result = AddVignette(result, ps_in.texCoords);
    }
    if (fgEnabled)
    {
        result = AddFilmGrain(result, ps_in.texCoords);
    }
    if (caEnabled)
    {
        result = AddChromaticAberration(result, ps_in.texCoords);
    }
    
    rw_FinalImage[uvs] = float4(result, 1.0);
}

float mod(float x, float y)
{
    return (x - y * floor(x / y));
}

float3 AddChromaticAberration(float3 sourceColor, float2 texCoords)
{
    float2 texel = 1.0 / cbScreenSize;
    float2 coords = (texCoords - 0.5) * 2.0;
    float coordsDot = dot(coords, coords);
    
    float2 precompute = caIntensity * coordsDot * coords;
    float2 uvR = texCoords - texel.xy * precompute;
    float2 uvB = texCoords + texel.xy * precompute;
    
    // TODO: this effect overwrites other effects because it adds the color texture directly. Fix it
    //sourceColor.r = t_LightPassResult.Sample(s_LinearWrapSampler, uvR).r;
    //sourceColor.g = t_LightPassResult.Sample(s_LinearWrapSampler, texCoords).g;
    //sourceColor.b = t_LightPassResult.Sample(s_LinearWrapSampler, uvB).b;
    
    return sourceColor;
}

float3 AddFilmGrain(float3 sourceColor, float2 texCoords)
{
    float x = (texCoords.x + 4.0) * (texCoords.y + 4.0) * (cbTime * 10.0);
    float grain = mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01) - 0.005;
    float4 grainAmount = float4(grain, grain, grain, grain) * fgStrength;
    
    grainAmount = 1.0 - grainAmount;
    return grainAmount.rgb * sourceColor;
}

float3 AddVignette(float3 sourceColor, float2 texCoords)
{
    float2 centerUV = texCoords - float2(0.5, 0.5);
    float3 color = float3(1.0, 1.0, 1.0);

    color.rgb *= 1.0 - smoothstep(vnInnerRadius, vnOuterRadius, length(centerUV));
    color *= sourceColor;
    color = lerp(sourceColor, color, vnOpacity);
    
    return color;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cbCameraNearZ * cbCameraFarZ) / (cbCameraFarZ + cbCameraNearZ - z * (cbCameraFarZ - cbCameraNearZ)) / cbCameraFarZ;
}
