#include "Bloom.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

float Knee(float x, float n, float k)
{
    float s = k / n * x;
    float e = (1.0f - k) / (1.0f - n) * (x - 1) + 1;
    return lerp(s, e, step(n, x));
}

PixelShaderOutPut main(VertexShaderOutPut input)
{
    PixelShaderOutPut output;
    float2 samplePoint = input.texcoord;
    output.color = tex.Sample(smp, input.texcoord);
    //float luminance = dot(output.color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    ////clip(luminance - param_.threshold);
    //output.color.xyz *= Knee(luminance, 0.85f, 0.3f);
    return output;
}