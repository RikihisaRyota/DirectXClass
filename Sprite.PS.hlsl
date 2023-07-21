#include "Sprite.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
};

ConstantBuffer<Material> gMaterial : register(b3);

struct DirectionalLight
{
    float4 color; //!< ライトの色
    float3 direction; //!< ライトの色
    float intensiy; //!< 輝度
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b4);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    if (gMaterial.enableLighting != 0)
    { // half lambert
        float Ndotl = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensiy;
    }
    else
    { // Lighingしない場合、前回までと同じ演算
        output.color = gMaterial.color * textureColor;
    }
    return output;
}