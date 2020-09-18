#include <../Common/Insight_Common.hlsli>
#include <Deferred_Rendering.hlsli>	

#define SHADOW_DEPTH_BIAS 0.00005f

// G-Buffer Inputs
// --------------
Texture2D t_AlbedoGBuffer               : register(t0);
Texture2D t_NormalGBuffer               : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer  : register(t2);
Texture2D t_PositionGBuffer             : register(t3);
Texture2D t_SceneDepthGBuffer           : register(t4);

Texture2D t_ShadowDepth         : register(t10);

TextureCube tc_IrradianceMap    : register(t11);
TextureCube tc_EnvironmentMap   : register(t12);
Texture2D t_BrdfLUT             : register(t13);

Texture2D t_RayTracePassResult : register(t16);


RWTexture2D<float4> rw_FinalImage : register(u1);


// Samplers
// --------
sampler s_PointClampSampler : register(s0);
sampler s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
void HDRToneMap(inout float3 Target);
void GammaCorrect(inout float3 Target);
float ShadowCalculation(float4 FragPosLightSpace, float3 Normal, float3 LightDir);

// Pixel Shader Return Value
// -------------------------
struct PS_OUTPUT_LIGHTPASS
{
    float3 Bloom : SV_Target;
};

// Entry Point
// -----------
PS_OUTPUT_LIGHTPASS main(PS_INPUT_LIGHTPASS ps_in)
{
    PS_OUTPUT_LIGHTPASS ps_out;
    
	// Sample Textures
    float3 albedo = pow(abs(t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb), float3(2.2, 2.2, 2.2));
    //ps_out.litImage = albedo;
    //return ps_out;
    float3 RoughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 WorldPosition = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float3 Normal = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float SceneDepth = t_SceneDepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float Roughness = RoughMetAOBufferSample.r;
    float Metallic = RoughMetAOBufferSample.g;
    float AmbientOcclusion = RoughMetAOBufferSample.b;
    
    float3 ViewDirection = normalize(cbCameraPosition - WorldPosition);
        
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 BaseReflectivity = lerp(F0, albedo, Metallic);
    float NdotV = max(dot(Normal, ViewDirection), 0.0000001);
    
    float3 SpotLightLuminance = float3(0.0, 0.0, 0.0);
    float3 PointLightLuminance = float3(0.0, 0.0, 0.0);
    float3 DirectionalLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < cbNumDirectionalLights; d++)
    {
        float3 LightDir = normalize(-dirLight.direction);
        
        // Shadowing
        float4 FragPosLightSpace = mul(float4(WorldPosition, 1.0), mul(dirLight.lightSpaceView, dirLight.lightSpaceProj));
        float Shadow = 1.0f;
        if (cbRayTraceEnabled)
        {
            Shadow = t_RayTracePassResult.Sample(s_PointClampSampler, ps_in.texCoords).r;
        }
        else
        {
            //Shadow = ShadowCalculation(FragPosLightSpace, Normal, LightDir);
        }
        
        DirectionalLightLuminance += CaclualteDirectionalLight(dirLight, ViewDirection, Normal, WorldPosition, NdotV, albedo, Roughness, Metallic, BaseReflectivity) * (Shadow);
    }
    
    // Spot Lights
    for (int s = 0; s < cbNumSpotLights; s++)
    {
        SpotLightLuminance += CalculateSpotLight(spotLights[s], ViewDirection, NdotV, WorldPosition, Normal, albedo, Roughness, Metallic, BaseReflectivity);
    }
    
    // Point Lights
    for (int p = 0; p < cbNumPointLights; p++)
    {
        PointLightLuminance += CalculatePointLight(pointLights[p], WorldPosition, ViewDirection, NdotV, Normal, albedo, Metallic, Roughness, BaseReflectivity);;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(NdotV, BaseReflectivity, Roughness);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - Metallic);
    float3 Diffuse_IBL = tc_IrradianceMap.Sample(s_LinearWrapSampler, Normal).rgb * albedo * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_MIP_LOD = 10.0f;
    float3 EnvironmentMapColor = tc_EnvironmentMap.SampleLevel(s_LinearWrapSampler, reflect(-ViewDirection, Normal), Roughness * MAX_REFLECTION_MIP_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearWrapSampler, float2(NdotV, Roughness)).rg;
    float3 Specular_IBL = EnvironmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 Ambient = (Diffuse_IBL + Specular_IBL) * (AmbientOcclusion);
    float3 CombinedLightLuminance = (PointLightLuminance + SpotLightLuminance) + (DirectionalLightLuminance);
    
     // Combine Light Luminance
    float3 PixelColor = Ambient + CombinedLightLuminance;
    
    HDRToneMap(PixelColor);
    GammaCorrect(PixelColor);
    
    float2 uvs = GetPixelCoords(ps_in.texCoords, cbScreenSize);
    rw_FinalImage[uvs] = float4(PixelColor, 1.0);
    
    ps_out.Bloom.rgb = float3(1.0, 0.0, 1.0);
    
    //float Brightness = dot(PixelColor, float3(0.2126, 0.7152, 0.0722));
    //if (Brightness > 1.0)
    //{
    //    ps_out.Bloom.rgb = PixelColor;
    //}
    //else
    //{
    //    ps_out.Bloom.rgb = float3(0.0, 0.0, 0.0);
    //}
    
    return ps_out;
}

void HDRToneMap(inout float3 Target)
{
    Target = float3(1.0, 1.0, 1.0) - exp(-Target * cbCameraExposure);
}

void GammaCorrect(inout float3 Target)
{
    const float Gamma = 2.2;
    Target = pow(abs(Target.rgb), float3(1.0 / Gamma, 1.0 / Gamma, 1.0 / Gamma));
}

float ShadowCalculation(float4 FragPosLightSpace, float3 Normal, float3 LightDir)
{
    float3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in Shadow
    float bias = max(0.05 * (1.0 - dot(Normal, LightDir)), 0.005);
    // Soften Shadows
    float Shadow = 0.0;
    float2 texelSize = 1.0 / float2(1024.0, 1024.0);
    [unroll(2)]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll(2)]
        for (int y = -1; y <= 1; ++y)
        {
            float depth = t_ShadowDepth.Sample(s_PointClampSampler, projCoords.xy + float2(x, y) * texelSize).r;
            Shadow += (depth + bias) < projCoords.z ? 0.0 : 1.0;

        }
    }
    return (1.0 - Shadow) / 9.0;
}
