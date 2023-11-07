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
    output.color = tex.Sample(smp, input.texcoord);
    output.color.rgb = float3(0.0f,0.0f,0.0f);
    
    return output;

}