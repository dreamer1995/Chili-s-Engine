cbuffer CBuf
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_V;
	matrix matrix_P;
	matrix matrix_VP;
	matrix matrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
};

cbuffer ObjectCBuf : register(b2)
{
	float time;
	float4 A;
	float4 S;
	float4 L;
	float4 w;
	float4 Q;
	float4 Dx;
	float4 Dz;
};

struct VSOut
{
	float3 worldPos : Position;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 bitangent : Binormal;
	float2 uv : Texcoord;
	float4 pos : SV_Position;
};

struct VSIn {
	float3 pos : Position;
	float3 n : Normal;
	float3 t : Tangent;
	float3 b : Binormal;
	float2 uv : Texcoord;
};

float4 CalculatePhase(float3 worldPos);
float3 CalculateWavesDisplacement(float3 worldPos, float4 sinp, float4 cosp);
float3 CalculateTangent(float4 sinp, float4 cosp);
float3 CalculateBinormal(float4 sinp, float4 cosp);

VSOut main(VSIn v)
{
	VSOut o;

	o.worldPos = (float3) mul(float4(v.pos, 1.0f), matrix_M2W);
	o.uv = v.uv;

	const float4 phase = CalculatePhase(o.worldPos);
	float4 sinp, cosp;
	sincos(phase, sinp, cosp);

	float3 disPos = CalculateWavesDisplacement(o.worldPos, sinp, cosp);

	//float depthmap = hmap.SampleLevel(splr, v.uv, 0.0f).r;
	float depthmap = (o.worldPos.z * 0.1f + 1.0f) * 0.5f;

	o.worldPos = o.worldPos + float3(disPos.x, disPos.y * depthmap, disPos.z);

	o.tangent = normalize(CalculateTangent(sinp, cosp));
	o.bitangent = -normalize(CalculateBinormal(sinp, cosp));

	o.tangent = normalize(lerp(normalize(mul(v.t, (float3x3)matrix_M2W)), o.tangent, depthmap));
	o.bitangent = normalize(lerp(normalize(mul(v.b, (float3x3)matrix_M2W)), o.bitangent, depthmap));
	o.normal = normalize(cross(o.tangent, o.bitangent));

	o.pos = float4(v.pos.xy * 0.2f, 0.0f, 1.0f);
	return o;
}

float4 CalculatePhase(float3 worldPos)
{
	float4 psi = S * w;
	return w * Dx * worldPos.x + w * Dz * worldPos.z + psi * time;
}

float3 CalculateWavesDisplacement(float3 worldPos, float4 sinp, float4 cosp)
{
	float3 Gpos;
	Gpos.x = worldPos.x + dot(Q * A * Dx, cosp);
	Gpos.z = worldPos.z + dot(Q * A * Dz, cosp);
	Gpos.y = dot(A, sinp);
	return Gpos;
}

float3 CalculateTangent(float4 sinp, float4 cosp)
{
	float3 Gtan;
	Gtan.x = 1.0f - dot(Q * A * w * Dx * Dx, sinp);
	Gtan.y = dot(A * w * Dx, cosp);
	Gtan.z = -dot(Q * A * w * Dz * Dx, sinp);
	return Gtan;
}

float3 CalculateBinormal(float4 sinp, float4 cosp)
{
	float3 GBin;
	GBin.x = -dot(Q * A * w * Dz * Dx, sinp);
	GBin.y = dot(A * w * Dz, cosp);
	GBin.z = 1.0f - dot(Q * A * w * Dz * Dz, sinp);
	return GBin;
}