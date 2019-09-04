cbuffer CBuf : register(b1)
{
	float4 color;
};

RasterizerState rsWireframe { FillMode = WireFrame; };

float4 main() : SV_Target
{
	return color;
}

technique11
{
	pass
	{
		SetRasterizerState(rsWireframe);
	}
};