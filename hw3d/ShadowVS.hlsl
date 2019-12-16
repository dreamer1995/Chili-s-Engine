cbuffer CBuf
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_V;
	matrix matrix_P;
	matrix matrixmatrix_VP;
	matrix matrixmatrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
};

cbuffer LightCBuf : register(b1)
{
	float3 direction;
	float3 diffuseColor;
	float diffuseIntensity;
	matrix lightSpaceVP;
};

float4 main( float3 pos : Position ) : SV_Position
{
	float4 worldPos = mul(float4(pos, 1.0f), matrix_M2W);
	return mul(worldPos, lightSpaceVP);
}