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
	float4 pos : SV_Position;
};

struct VSIn 
{
	float3 pos : Position;
	float3 n : Normal;
};

VSOut main(VSIn v)
{
    VSOut o;
	o.pos = mul(float4(v.pos, 1.0f), matrix_MVP);
	o.worldPos = (float3) mul(float4(v.pos, 1.0f), matrix_M2W);
	o.normal = normalize(mul(v.n, (float3x3)matrix_M2W));
    return o;
}