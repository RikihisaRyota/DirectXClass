#include "Line.hlsli"

struct ViewProjection
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラのワールド座標
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    output.position = mul(output.position, mul(gViewProjection.view, gViewProjection.projection));
    output.color = input.color;
    return output;
}