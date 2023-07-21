#include "Pera.hlsli"

//Texture2D<float4> tex : register(t0); // 通常テクスチャ
//SamplerState smp : register(s0); // サンプラー

//struct Output
//{
//    float4 svpos : SV_POSITION;
//    float2 uv : TEXCOORD;
//};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

Output main(VertexShaderInput input)
{
    Output output;
    output.svpos = input.position;
    output.uv = input.texcoord;
    return output;
}