#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

LightPixelInput VS(VertexInstancing input)
{
	LightPixelInput output;
	
	matrix transform = mul(SkinWorld(input.index, input.indices, input.weights), input.transform);
	
	output.pos = mul(input.pos, transform * 1.5f);
	
	output.viewPos = invView._41_42_43;
	output.worldPos = output.pos;
	
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.uv = input.uv;
	
	return output;
}

float4 PS(LightPixelInput input) : SV_TARGET
{
    return float4(1.f,0.f,1.f,0.2f);
}