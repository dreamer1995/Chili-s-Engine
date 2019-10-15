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
	float2 offset;
};

Texture2D mnmap : register(t20);
Texture2D snmap : register(t21);
SamplerState splr;

struct VSOut
{
	float3 oldPos : Position;
	float3 newPos : Normal;
	float4 pos : SV_Position;
};

float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed);

VSOut main(float3 pos : Position, float2 uv : Texcoord)
{
	VSOut o;
	o.oldPos = (float3)mul(float4(pos, 1.0f), matrix_M2W);

	float3 mediumWaves = Motion_4WayChaos_Normal(mnmap, uv, 0.5f);
	float3 smallWaves = Motion_4WayChaos_Normal(snmap, uv, 0.5f);
	float3 mixWaves = (mediumWaves + smallWaves) * 0.5f;

	float3 bumpNormal = mixWaves;
	bumpNormal = normalize(mul(bumpNormal * 2.0f - 1.0f, (float3x3)matrix_M2W));
	const float NdotL = max(dot(bumpNormal, direction), 0.0f);
	const float t = lerp(0.225f, 0.465f, NdotL);
	const float3 Rv = lerp(-direction, -bumpNormal, t);
	//const float n = 0.75f;
	//const float c1 = dot(bumpNormal, -direction);
	//const float c2 = sqrt(1 - n * n*(1.0f - c1 * c1));
	//const float3 Rv = n * -direction + (n * c1 - c2) * bumpNormal;

	float4 worldPos = mul(float4(pos, 1.0f), matrix_M2W) + float4(Rv.x * depth, 0.0f, Rv.z * depth, 0.0f);
	o.newPos = (float3)worldPos;
	o.pos = float4(mul(worldPos, matrix_W2M).xy + offset, 0.0f, 1.0f);
	return o;
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