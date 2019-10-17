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
	float depth;
	float time;
	float2 offset;
	float speed;
	float roughness;
	float flatten1;
	float flatten2;
};

Texture2D rmap : register(t20);
Texture2D mnmap : register(t21);
Texture2D snmap : register(t22);
Texture2D hmap : register(t23);
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

float Motion_4WayChaos(Texture2D textureIn, float2 uv, float speed);
float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed);

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

	float fRoughness = Motion_4WayChaos(rmap, uv * 4.0f, 0.05f);
	float variationAmount;
	fRoughness += 0.5f;
	float variationSharpness = 13.0511389f;
	fRoughness = pow(fRoughness, variationSharpness) * variationSharpness;
	fRoughness = saturate(fRoughness);
	fRoughness = roughness * lerp(0.164602f, 0.169983f, fRoughness);

	float3 mediumWaves = lerp(Motion_4WayChaos_Normal(mnmap, uv, speed), float3(0.5f, 0.5f, 1.0f), flatten1);
	float3 smallWaves = lerp(Motion_4WayChaos_Normal(snmap, uv, speed), float3(0.5f, 0.5f, 1.0f), flatten2);
	float3 mixWaves = (mediumWaves + smallWaves) * 0.5f;
	float waveletNormalStrength = 0.034862f;
	float3 wavelet = lerp(float3(0.5f, 0.5f, 1.0f), Motion_4WayChaos_Normal(snmap, uv * 0.5f, 0.40708f), waveletNormalStrength);

	float3 bumpNormal = lerp(mixWaves, wavelet, fRoughness);
	bumpNormal = normalize(mul(bumpNormal * 2.0f - 1.0f, (float3x3)matrix_M2W));
	const float NdotL = max(dot(bumpNormal, float3(0.0f, 1.0f, 0.0f)), 0.0f);
	const float t = lerp(0.225f, 0.465f, NdotL);
	const float3 Rv = lerp(float3(0.0f, -1.0f, 0.0f), -bumpNormal, t);
	//const float n = 0.75f;
	//const float c1 = dot(bumpNormal, -direction);
	//const float c2 = sqrt(1 - n * n*(1.0f - c1 * c1));
	//const float3 Rv = n * -direction + (n * c1 - c2) * bumpNormal;

	float depthmap = hmap.SampleLevel(splr, uv, 0.0f).r;

	float4 worldPos = mul(pos, matrix_M2W) + float4(Rv.x * (depth * depthmap + 0.1f), 0.0f, Rv.z * (depth * depthmap + 0.1f), 0.0f);
	o.oldPos = (float3)mul(pos, matrix_M2W);
	o.newPos = (float3)worldPos;

	o.pos = float4(mul(worldPos, matrix_W2M).xy + offset, 0.0f, 1.0f);
	//o.pos = mul(float4(o.newPos, 1.0f), matrix_VP);

	return o;
}

float Motion_4WayChaos(Texture2D textureIn, float2 uv, float speed)
{
	float2 offset[4] = { float2(0.000000f,0.000000f),
						 float2(0.418100f,0.354800f),
						 float2(0.864861f,0.148384f),
						 float2(0.651340f,0.751638f) };
	float2 pannerDir[4] = { float2(0.1f,0.1f),
							float2(-0.1f,-0.1f),
							float2(-0.1f,0.1f),
							float2(0.1f,-0.1f) };
	float outPut = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		float textureSample = textureIn.SampleLevel(splr, uv + offset[i] + pannerDir[i] * time * speed, 0.0f).r;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}

float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed)
{
	float2 offset[4] = { float2(0.000000f,0.000000f),
						 float2(0.418100f,0.354800f),
						 float2(0.864861f,0.148384f),
						 float2(0.651340f,0.751638f) };
	float2 pannerDir[4] = { float2(0.1f,0.1f),
							float2(-0.1f,-0.1f),
							float2(-0.1f,0.1f),
							float2(0.1f,-0.1f) };
	float3 outPut = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		float3 textureSample = textureIn.SampleLevel(splr, uv + offset[i] + pannerDir[i] * time * speed, 0.0f).rgb;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}