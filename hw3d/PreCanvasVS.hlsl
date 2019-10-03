cbuffer CBuf
{
	matrix matrix_MVP;
};

struct VSOut
{
	float2 tc : Texcoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tc : Texcoord)
{
	VSOut vso;
	vso.tc = tc;
	vso.pos = float4(pos.xy, 0.0f, 1.0f);
	return vso;
}