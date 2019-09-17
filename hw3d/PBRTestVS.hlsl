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
	//float3 tangent : Tangent;
	//float3 bitangent : Binormal;
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};

struct VSIn {
	float3 pos : Position;
	float3 n : Normal;
	//float3 t : Tangent;
	//float3 b : Binormal;
	float2 uv : Texcoord;
};

VSOut main(VSIn v)
{
	VSOut o;
	o.pos = mul(float4(v.pos, 1.0f), matrix_MVP);
	o.worldPos = (float3) mul(float4(v.pos, 1.0f), matrix_M2W);
	o.normal = normalize(mul(v.n, (float3x3)matrix_M2W));
	//o.tangent = normalize(mul(v.t, (float3x3)matrix_M2W));
	//o.bitangent = normalize(mul(v.b, (float3x3)matrix_M2W));
	o.uv = v.uv;
	return o;
}