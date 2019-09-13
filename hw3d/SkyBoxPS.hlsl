cbuffer CBuf : register(b2)
{
	float4 color;
};

TextureCube SkyMap;
SamplerState splr;

float4 main(float3 tc : Texcoord) : SV_Target
{
	return SkyMap.Sample(splr, normalize(tc)) * color;
}

//RasterizerState MyCull {
//	FillMode = WireFrame;
//};
//
//technique11 main11
//{
//	pass p0
//	{
//		SetRasterizerState(MyCull);
//	}
//};