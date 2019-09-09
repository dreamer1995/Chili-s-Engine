cbuffer CBuf
{
	matrix matrix_MVP;
};

float4 main( float3 pos : Position ) : SV_Position
{
	return mul( float4(pos,1.0f),matrix_MVP);
}