#include "VerticalBlur.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VertexShaderInput input)
{
    VSOutput output;

    float2 textureSize;
    float level;
    tex.GetDimensions(0, textureSize.x, textureSize.y, level);
    
    float2 base = input.texcoord;
  
    float2 texelSize = float2(0.0f, 1.0f / textureSize.y);
    
    output.position = float4(lerp(float2(-1.0f, 1.0f), float2(1.0f, -1.0f), base), 0.0f, 1.0f);
   
    float2 offset = texelSize;
    float4 basebase = float4(base, base);
    
    for (uint i = 0; i < 8; ++i)
    {
        output.texcoord[i].xy = offset;
        output.texcoord[i].zw = output.texcoord[i].xy * -1.0f;
        output.texcoord[i] += basebase;
        offset = texelSize * 2.0f;
    }
    
    return output;
}