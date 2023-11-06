
struct VertexShaderOutPut
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

float4 main(VertexShaderOutPut input)
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}