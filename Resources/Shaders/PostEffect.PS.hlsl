Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー


struct VertexShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};


PixelShaderOutPut main(VertexShaderInput input)
{
    PixelShaderOutPut output;
    float2 samplePoint = input.texcoord;
    output.color = tex.Sample(smp, samplePoint);
    samplePoint.x += 0.01;
    output.color.r = tex.Sample(smp, samplePoint).r;
    return output;
}