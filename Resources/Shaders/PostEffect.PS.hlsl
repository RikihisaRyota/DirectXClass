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
    float2 samplePoint = input.texcoord;
    output.color = tex.Sample(smp, input.texcoord);
    // 反転
    //output.color.rgb = (1.0f - output.color.r, 1.0f - output.color.g, 1.0f - output.color.b);
    // 白黒
    //float averageColor = (output.color.r + output.color.g + output.color.b) / 3.0f;
    //output.color.rgb = (averageColor, averageColor, averageColor);
    // ノイズ
    float noise = Random(input.texcoord*gTime.time) - 0.5f;
    output.color.rgb += float3(noise, noise, noise);
    // ゆがませる
    static const float Distortion = 0.05f;
    samplePoint -= float2(0.5f, 0.5f);
    float distPower = pow(length(samplePoint), Distortion);
    samplePoint *= float2(distPower, distPower);
    samplePoint += float2(0.5f, 0.5f);
    output.color = tex.Sample(smp, samplePoint);
    // RGBずらし
    samplePoint = input.texcoord;
    samplePoint.r += 0.01;
    output.color.r = tex.Sample(smp, samplePoint).r;
    // 走査線
    float sinv = sin(input.texcoord.y * 2 + gTime.time * -0.1);
    float steped = step(0.99, sinv * sinv);
    output.color.rgb -= (1 - steped) * abs(sin(input.texcoord.y * 50.0 + gTime.time * 1.0)) * 0.05;
    output.color.rgb -= (1 - steped) * abs(sin(input.texcoord.y * 100.0 - gTime.time * 2.0)) * 0.08;
    output.color.rgb += steped * 0.1;
    // ビネット
    float vignette = length(float2(0.5, 0.5) - input.texcoord);
    vignette = clamp(vignette - 0.2, 0, 1);
    output.color.rgb -= vignette;
    return output;
}