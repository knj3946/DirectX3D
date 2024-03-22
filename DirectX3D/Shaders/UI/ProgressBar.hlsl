#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

PixelInput VS(VertexUV input)
{
	PixelInput output;
	output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.uv = input.uv;
	
	return output;
}

cbuffer ValueBuffer : register(b10)
{
	float fillAmount;
}

cbuffer BlendAlpha : register(b13) {
	float Blend;
}

Texture2D backImage : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{
	float4 vColor;
	if (input.uv.x < fillAmount)
	{
		vColor = diffuseMap.Sample(samp, input.uv);;
	//	vColor.a = Blend;
		clip(vColor.a - 0.1f);
		vColor.a = Blend;
		return vColor;
	}
	vColor = backImage.Sample(samp, input.uv);
	//vColor.a = Blend;
	clip(vColor.a - 0.1f);
	vColor.a = Blend;
	return vColor;
}