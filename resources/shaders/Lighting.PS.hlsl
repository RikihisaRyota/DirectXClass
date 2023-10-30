#include "Lighting.hlsli"

Texture2D<float4> gTexture : register(t0);
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

struct DirectionLight
{
    float4 color;
    float3 direction;
    float intensity;
    float sharpness;
};

ConstantBuffer<DirectionLight> gDirectionLight : register(b3);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTranslate);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (textureColor.a <= 0.5f || textureColor.a == 0.0f)
    {
        discard;
    }
    if (gMaterial.enableLighting == 1)
    { 
        // Half ranbert
        // ディフーズ
        float NdotL = saturate(dot(normalize(input.normal), -gDirectionLight.direction));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        float4 diffuse = gDirectionLight.color * cos * gDirectionLight.intensity;
        // スペキュラー
        float3 refVec = reflect(gDirectionLight.direction, input.normal);
        float3 toEye = normalize(gViewProjection.cameraPos - input.ray.xyz);
        float t = saturate(dot(refVec, toEye));
        t = pow(t, 5.0f);
        float4 specular = gDirectionLight.color * t * gDirectionLight.sharpness;
        // アンビエント
        float4 ambient = float4(0.1f, 0.1f, 0.1f, 0.0f);
        output.color = gMaterial.color * textureColor * (diffuse + specular + ambient);
    }
    else if (gMaterial.enableLighting == 2)
    {
        // ranbert
        float cos = saturate(dot(normalize(input.normal), -gDirectionLight.direction));
        float4 diffuse = gDirectionLight.color * cos * gDirectionLight.intensity;
         // スペキュラー
        float3 refVec = reflect(gDirectionLight.direction, input.normal);
        float3 toEye = normalize(gViewProjection.cameraPos - input.ray.xyz);
        float t = saturate(dot(refVec, toEye));
        t = pow(t, 5.0f);
        float4 specular = gDirectionLight.color * t * gDirectionLight.sharpness;
        // アンビエント
        float4 ambient = float4(0.1f, 0.1f, 0.1f, 0.0f);
        output.color = gMaterial.color * textureColor * (diffuse + specular + ambient);
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    //if (output.color.a == 0.0f)
    //{
    //    discard;
    //}
    return output;
}
