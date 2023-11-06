
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct VertexShaderOutPut
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    return input;
}