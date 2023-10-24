#include "Particle.hlsli"

Texture2D<float4> gTexture : register(t1);
SamplerState gSampler : register(s0);

struct ViewProjection
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラのワールド座標
};

ConstantBuffer<ViewProjection> gViewProjection : register(b1);

struct Material
{
    float4 color;
    uint enableLighting;
    matrix uvTranslate;
};

ConstantBuffer<Material> gMaterial : register(b2);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTranslate);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (textureColor.a == 0.0f)
    {
        discard;
    }

    output.color = gMaterial.color * textureColor;
    
    return output;
}
