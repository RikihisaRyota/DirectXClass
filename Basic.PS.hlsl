#include "Basic.hlsli"

Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gToonTexture : register(t1);
SamplerState gSampler : register(s0);

struct CullingFlag
{
    int flag;
};

ConstantBuffer<CullingFlag> gCullingFlag : register(b2);

struct Material
{
    float4 color;
    int enableLighting;
};

ConstantBuffer<Material> gMaterial : register(b3);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b4);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    if (!gCullingFlag.flag)
    {
        output.color = float4(0, 0, 0, 1);
    }
    else
    {
        float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
        float p = dot(input.normal * -1.0f, gDirectionalLight.direction);
        p = p * 0.5f + 0.5f;
        p = p * p;
        float4 Col = gToonTexture.Sample(gSampler, float2(p, 0.0f));
        float NdotL = dot(input.normal, -gDirectionalLight.direction);
        float lightingIntensity = saturate(NdotL);
        output.color = gMaterial.color * textureColor * float4(Col.xyz, 1.0f) * lightingIntensity * gDirectionalLight.intensity;
    }
    
    return output;
}
