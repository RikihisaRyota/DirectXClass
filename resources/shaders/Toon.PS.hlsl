#include "VertexShaderOutput.hlsli"

Texture2D<float4> gToonTexture : register(t0);
SamplerState gToonSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
};

ConstantBuffer<Material> gMaterial : register(b2);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    float sharpness;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b3);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float3 CalculateToonShading(float3 normal, float3 lightDir, float intensity, float toonLevels)
{
    float3 reflectedLight = reflect(-lightDir, normal);
    float specular = saturate(dot(reflectedLight, normalize(-gDirectionalLight.direction)));
    float3 shadedColor = gToonTexture.Sample(gToonSampler, float2(specular, intensity / toonLevels)).rgb;
    return shadedColor;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // オブジェクトの色
    float4 objectColor = gMaterial.color;

    // ライティングを適用する場合
    if (gMaterial.enableLighting != 0)
    {
        // ライト方向
        float3 lightDir = normalize(-gDirectionalLight.direction);
        // ライティング強度
        float intensity = saturate(dot(input.normal, lightDir));

        // カスタムトゥーンテクスチャーの階調数（階調の数を調整することで影の質感を変更可能）
        const float toonLevels = 2.0f;

        // トゥーンシェーディングを計算
        float3 toonShading = CalculateToonShading(input.normal, lightDir, intensity, toonLevels);

        // 最終出力色にトゥーンシェーディングを適用
        output.color = float4(objectColor.rgb * toonShading, objectColor.a);
        ////////////////////////////////////////////////////////////////////
        output.color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else
    {
        // ライティングを無効にしてオブジェクトの色を出力
        output.color = objectColor;
        ////////////////////////////////////////////////////////////////////
        output.color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    }

    return output;
}
