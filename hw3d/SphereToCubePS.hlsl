Texture2D SkyMap;
SamplerState splr;

static float2 invAtan = float2(0.1591f, 0.3182f);
float2 SampleSphereicalMap(float3 tc)
{
	float2 uv = float2(atan2(tc.z, tc.x), asin(-tc.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

float4 main(float3 tc : Texcoord) : SV_Target
{
	float2 uv = SampleSphereicalMap(normalize(tc));
	return SkyMap.Sample(splr, uv);
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