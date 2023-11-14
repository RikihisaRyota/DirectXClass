#include "Bloom.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutPut main(VertexShaderInput input)
{
    VertexShaderOutPut output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    return output;
}