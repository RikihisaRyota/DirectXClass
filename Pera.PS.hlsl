Texture2D<float4> tex : register(t0); // 通常テクスチャ
SamplerState smp : register(s0); // サンプラー

struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};
float4 main(Output input) : SV_TARGET
{
    return float4(input.uv, 0.0f, 1.0f);
}