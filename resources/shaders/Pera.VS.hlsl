#include "Pera.hlsli"

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