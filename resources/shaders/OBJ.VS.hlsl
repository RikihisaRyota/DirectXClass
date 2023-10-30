#include "OBJ.hlsli"

struct WorldTransform
{
    matrix world; // ワールド変換行列
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);

struct ViewProjection
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラのワールド座標
};

ConstantBuffer<ViewProjection> gViewProjection : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output; // ピクセルシェーダーに渡す値
    output.position = mul(mul(input.position, gWorldTransform.world), mul(gViewProjection.view, gViewProjection.projection));
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    return output;
}