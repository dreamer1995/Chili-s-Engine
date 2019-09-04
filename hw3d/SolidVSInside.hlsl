cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProj;
};

RasterizerState rsWireframe { FillMode = WireFrame; };

float4 main( float3 pos : Position ) : SV_Position
{
	return mul( float4(pos,1.0f),modelViewProj );
}

technique11
{
	pass
	{
		SetRasterizerState(rsWireframe);
	}
};