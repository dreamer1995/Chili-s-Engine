#include <PBRHeader.hlsli>
cbuffer ObjectCBuf : register(b5)
{
	float _time;
	float speed;
	float __roughness;
	float flatten1;
	float flatten2;
	bool _normalMapEnabled;
};

Texture2D rmap;
Texture2D mnmap : register(t1);
Texture2D snmap : register(t2);

struct PSIn {
	float3 worldPos : Position;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;
	float2 uv : Texcoord;
};

float Motion_4WayChaos(Texture2D textureIn, float2 uv, float speed);
float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed);

float4 main(PSIn i) : SV_Target
{
	float fRoughness = Motion_4WayChaos(rmap, i.uv * 4.0f, 0.05f);
	float variationAmount;
	fRoughness += 0.5f;
	float variationSharpness = 13.0511389f;
	fRoughness = pow(fRoughness, variationSharpness) * variationSharpness;
	fRoughness = saturate(fRoughness);
	fRoughness = __roughness * lerp(0.164602f, 0.169983f, fRoughness);
	// sample normal from map if normal mapping enabled	
	if (_normalMapEnabled)
	{
		float3 mediumWaves = lerp(Motion_4WayChaos_Normal(mnmap, i.uv, speed), float3(0.5f, 0.5f, 1.0f), flatten1);
		float3 smallWaves = lerp(Motion_4WayChaos_Normal(snmap, i.uv, speed), float3(0.5f, 0.5f, 1.0f), flatten2 + 0.65f);
		float3 mixWaves = (mediumWaves + smallWaves) * 0.5f;
		float waveletNormalStrength = 0.034862f;
		float3 wavelet = lerp(float3(0.5f, 0.5f, 1.0f), Motion_4WayChaos_Normal(snmap, i.uv * 0.5f, 0.40708f), waveletNormalStrength);

		float3 bumpNormal = lerp(mixWaves, wavelet, fRoughness);
		bumpNormal = bumpNormal * 2.0f - 1.0f;
		bumpNormal = (bumpNormal.x * normalize(i.tangent)) + (bumpNormal.y * normalize(i.binormal)) + (bumpNormal.z * normalize(i.normal));
		i.normal = normalize(bumpNormal);
	}
	else
	{
		i.normal = normalize(i.normal);
	}

	return float4((i.normal + 1.0f) * 0.5f, 1.0f);

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
		float textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * _time * speed).r;
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
		float3 textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * _time * speed).rgb;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}

float2 UVRefractionDistorted(float3 Rv, float2 uv, float depth)
{
	return float2(uv.x + Rv.x * depth, uv.y + Rv.z * depth);
}