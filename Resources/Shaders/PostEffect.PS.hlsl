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


struct Time
{
    float time;
};

ConstantBuffer<Time> gTime : register(b0);

float Random(float2 coord)
{
    return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}


PixelShaderOutPut main(VertexShaderInput input)
{
    PixelShaderOutPut output;
    
    output.color = tex.Sample(smp, input.texcoord);
    // 反転
    //output.color.rgb = (1.0f - output.color.r, 1.0f - output.color.g, 1.0f - output.color.b);
    // 白黒
    //float averageColor = (output.color.r + output.color.g + output.color.b) / 3.0f;
    //output.color.rgb = (averageColor, averageColor, averageColor);
    // RGBずらし
    //float2 samplePoint = input.texcoord;
    //samplePoint.x += 0.01;
    //output.color.r = tex.Sample(smp, samplePoint).r;
    
    //float noise = Random(input.texcoord * gTime.time) - 0.5f;
    //output.color.rgb = float3(noise, noise, noise);
    return output;
}