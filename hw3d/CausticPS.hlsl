cbuffer CBuf : register(b3)
{
	float4 color;
};

struct PSIn {
	float3 oldPos : Position;
	float3 newPos : Position1;
};

float4 main(PSIn i) : SV_Target
{
	float oldArea = length(ddx(i.oldPos)) * length(ddy(i.oldPos));
	float newArea = length(ddx(i.newPos)) * length(ddy(i.newPos));
	return color * oldArea / newArea;
}