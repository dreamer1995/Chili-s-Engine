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
	float3 color;
	float depth;
	float time;
};

Texture2D rmap : register(t20);
Texture2D mnmap : register(t21);
Texture2D snmap : register(t22);
SamplerState splr;

struct VSOut
{
	float3 oldPos : Position;
	float3 newPos : Normal;
	float4 pos : SV_Position;
};

float Motion_4WayChaos(Texture2D textureIn, float2 uv, float speed);
float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed);

VSOut main(float3 pos : Position, float2 uv : Texcoord)
{
	VSOut o;
	o.oldPos = (float3)mul(float4(pos, 1.0f), matrix_M2W);
	float fRoughness = Motion_4WayChaos(rmap, uv * 4.0f, 0.05f);
	float variationAmount;
	fRoughness += 0.5f;
	float variationSharpness = 13.0511389f;
	fRoughness = pow(fRoughness, variationSharpness) * variationSharpness;
	fRoughness = saturate(fRoughness);
	fRoughness = lerp(0.164602f, 0.169983f, fRoughness);

	float3 mediumWaves = Motion_4WayChaos_Normal(mnmap, uv, 0.5f);
	float3 smallWaves = Motion_4WayChaos_Normal(snmap, uv, 0.5f);
	float3 mixWaves = (mediumWaves + smallWaves) * 0.5f;
	float waveletNormalStrength = 0.034862f;
	float3 wavelet = lerp(float3(0.0f, 0.0f, 1.0f), Motion_4WayChaos_Normal(snmap, uv * 0.5f, 0.40708f), waveletNormalStrength);

	float3 bumpNormal = lerp(mixWaves, wavelet, fRoughness);
	bumpNormal = normalize(mul(bumpNormal * 2.0f - 1.0f, (float3x3)matrix_M2W));
	const float NdotL = max(dot(bumpNormal, direction), 0.0f);
	const float t = lerp(0.225f, 0.465f, NdotL);
	const float3 Rv = lerp(-direction, -bumpNormal, t);
	//const float n = 0.75f;
	//const float c1 = dot(bumpNormal, -direction);
	//const float c2 = sqrt(1 - n * n*(1.0f - c1 * c1));
	//const float3 Rv = n * -direction + (n * c1 - c2) * bumpNormal;

	float4 worldPos = mul(float4(pos, 1.0f), matrix_M2W) + float4(Rv.x * depth, 0.0f, Rv.z * depth, 0.0f);
	o.pos= mul(worldPos, matrix_VP);
	o.newPos = (float3)o.pos;
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