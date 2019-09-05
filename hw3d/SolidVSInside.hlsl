cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProj;
};

struct VSOut
{
	float3 tc : Texcoord;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : Position )
{
	VSOut vso;
	vso.tc = pos;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.pos = vso.pos.xyww;
	return vso;
}
