#include "VerticalBlur.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

struct Param
{
    float4 weight[2];
};
ConstantBuffer<Param> param : register(b0);

PixelShaderOutPut main(VSOutput input)
{
    PixelShaderOutPut output;
    
    output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    output.color += param.weight[0].x * tex.Sample(smp, input.texcoord[0].xy);
    output.color += param.weight[0].y * tex.Sample(smp, input.texcoord[1].xy);
    output.color += param.weight[0].z * tex.Sample(smp, input.texcoord[2].xy);
    output.color += param.weight[0].w * tex.Sample(smp, input.texcoord[3].xy);
    output.color += param.weight[1].x * tex.Sample(smp, input.texcoord[4].xy);
    output.color += param.weight[1].y * tex.Sample(smp, input.texcoord[5].xy);
    output.color += param.weight[1].z * tex.Sample(smp, input.texcoord[6].xy);
    output.color += param.weight[1].w * tex.Sample(smp, input.texcoord[7].xy);
    
    output.color += param.weight[0].x * tex.Sample(smp, input.texcoord[0].zw);
    output.color += param.weight[0].y * tex.Sample(smp, input.texcoord[1].zw);
    output.color += param.weight[0].z * tex.Sample(smp, input.texcoord[2].zw);
    output.color += param.weight[0].w * tex.Sample(smp, input.texcoord[3].zw);
    output.color += param.weight[1].x * tex.Sample(smp, input.texcoord[4].zw);
    output.color += param.weight[1].y * tex.Sample(smp, input.texcoord[5].zw);
    output.color += param.weight[1].z * tex.Sample(smp, input.texcoord[6].zw);
    output.color += param.weight[1].w * tex.Sample(smp, input.texcoord[7].zw);
    
    output.color.w = 1.0f;
    
    return output;
}