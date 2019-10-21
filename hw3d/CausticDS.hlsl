// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer TransformCBuf
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

cbuffer CBufLight : register(b1)
{
	float3 direction;
};

cbuffer CBufProperties : register(b2)
{
	float4 A;
	float4 S;
	float4 L;
	float4 w;
	float4 Q;
	float4 Dx;
	float4 Dz;
	float depth;
	float time;
};

Texture2D hmap : register(t20);
Texture2D nmap : register(t21);
SamplerState splr;

struct DS_OUTPUT
{
	float3 oldPos : Position;
	float3 newPos : Position1;
	float4 pos : SV_POSITION;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 pos : Position;
	float2 uv : Texcoord;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2] : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

float4 CalculatePhase(float3 worldPos);
float CalculateWavesDisplacement(float4 sinp);

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT o;

	float3 topMidpoint = lerp(patch[0].pos, patch[1].pos, domain.y);
	float3 bottomMidpoint = lerp(patch[3].pos, patch[2].pos, domain.y);

	const float4 pos = float4(lerp(topMidpoint, bottomMidpoint, domain.x), 1.0f);

	float2 _topMidpoint = lerp(patch[0].uv, patch[1].uv, domain.y);
	float2 _bottomMidpoint = lerp(patch[3].uv, patch[2].uv, domain.y);

	const float2 uv = lerp(_topMidpoint, _bottomMidpoint, domain.x);

	o.oldPos = (float3)mul(pos, matrix_M2W);

	const float4 phase = CalculatePhase(o.oldPos);
	float4 sinp, cosp;
	sincos(phase, sinp, cosp);

	float disPosY = CalculateWavesDisplacement(sinp);

	float depthR = o.oldPos.y + disPosY + depth;

	float shootScale = 1.0f;
	float _shootScale = 1.0f / shootScale;

	float3 bumpNormal = nmap.SampleLevel(splr, (uv) * shootScale, 0.0f).rgb;
	bumpNormal = bumpNormal * 2.0f - 1.0f;

	const float NdotL = max(dot(bumpNormal, float3(0.0f, 1.0f, 0.0f)), 0.0f);
	const float t = lerp(0.225f, 0.465f, NdotL);
	const float3 Rv = lerp(float3(0.0f, -1.0f, 0.0f), -bumpNormal, t);
	//const float n = 0.75f;
	//const float c1 = dot(bumpNormal, -direction);
	//const float c2 = sqrt(1 - n * n*(1.0f - c1 * c1));
	//const float3 Rv = n * -direction + (n * c1 - c2) * bumpNormal;

	//float depthmap = hmap.SampleLevel(splr, (uv) * shootScale, 0.0f).r;
	float depthmap = (o.oldPos.z + 1.0f) * 0.5f;

	float4 worldPos = mul(pos, matrix_M2W) + float4(Rv.x * (depthR * depthmap) * _shootScale, 0.0f,
													Rv.z * (depthR * depthmap) * _shootScale, 0.0f);

	o.newPos = (float3)worldPos;

	o.pos = float4(mul(worldPos, matrix_W2M).xy * shootScale, 0.0f, 1.0f);
	//o.pos = mul(float4(o.newPos, 1.0f), matrix_VP);

	return o;
}

float4 CalculatePhase(float3 worldPos)
{
	float4 psi = S * w;
	return w * Dx * worldPos.x + w * Dz * worldPos.z + psi * time;
}

float CalculateWavesDisplacement(float4 sinp)
{
	return dot(A, sinp);
}