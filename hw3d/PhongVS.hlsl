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

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), matrix_MV);
    vso.normal = mul(n, (float3x3) matrix_MV);
    vso.pos = mul(float4(pos, 1.0f), matrix_MVP);
    vso.tc = tc;
    return vso;
}