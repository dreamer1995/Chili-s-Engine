#include <PBRHeader.hlsli>
cbuffer ObjectCBuf : register(b5)
{

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
	float fRoughness = _roughness * Motion_4WayChaos(rmap, i.uv * 4.0f, 0.05f);
	float variationAmount;
	fRoughness += 0.5f;
	float variationSharpness = 13.0511389f;
	fRoughness = pow(fRoughness, variationSharpness) * variationSharpness;
	fRoughness = saturate(fRoughness);
	fRoughness = lerp(0.164602f, 0.169983f, fRoughness);
	// sample normal from map if normal mapping enabled	
	if (normalMapEnabled)
	{
		float3 mediumWaves = Motion_4WayChaos_Normal(mnmap, i.uv, 0.5f);
		float3 smallWaves = Motion_4WayChaos_Normal(snmap, i.uv, 0.5f);
		float3 mixWaves = (mediumWaves + smallWaves) * 0.5f;
		float waveletNormalStrength = 0.034862f;
		float3 wavelet = lerp(float3(0.0f, 0.0f, 1.0f), Motion_4WayChaos_Normal(snmap, i.uv * 0.5f, 0.40708f), waveletNormalStrength);

		float3 bumpNormal = lerp(mixWaves, wavelet, fRoughness);
		bumpNormal = bumpNormal * 2.0f - 1.0f;
		bumpNormal = (bumpNormal.x * normalize(i.tangent)) + (bumpNormal.y * normalize(i.binormal)) + (bumpNormal.z * normalize(i.normal));
		i.normal = normalize(bumpNormal);
	}
	else
	{
		i.normal = normalize(i.normal);
	}

	//const float3 PlightDir = normalize(lightPos - i.worldPos);

	//const float distToL = length(lightPos - i.worldPos);

	const float3 viewDir = normalize(cameraPos - i.worldPos);
	//PBR Start
	const float NdotV = max(dot(i.normal, viewDir), 0.0f);
	const float NdotL = max(dot(i.normal, direction), 0.0f);
	const float3 halfDir = normalize(direction + viewDir);
	float NdotH = max(dot(i.normal, halfDir), 0.0f);
	float3 rotatedNormal = normalize(mul(i.normal, (float3x3)EVRotation));
	float3 R = reflect(normalize(mul(-viewDir, (float3x3)EVRotation)), rotatedNormal);
	//float3 albedo = tex.Sample(splr, i.uv).rgb * color;
	//float3(1.0f, 0.0f, 0.0f)
	
	float fresnel = 1 - NdotV;
	float3 albedo = lerp(pow(float3(0.018450f, 0.045000f, 0.042473f), 2.2f),
						 pow(float3(0.162565f, 0.271166f, 0.325000f), 2.2f),
						 fresnel);
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	float fMetallic = metallic * 0.8f;
	F0 = lerp(F0, albedo, fMetallic);
	//fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * albedo;

	//const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

	const float3 radiance = DdiffuseColor * DdiffuseIntensity;

	float NDF = DistributionGGX(NdotH, fRoughness);
	float G = GeometrySmith(NdotV, NdotL, fRoughness);
	float3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0f), F0);

	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - fMetallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0f * NdotV * NdotL + 0.001f;
	float3 specular = numerator / denominator;

	float3 Light = (kD * albedo / PI + specular) * radiance * NdotL;

	float3 iKS = FresnelSchlickRoughness(NdotV, F0, fRoughness);
	float3 iKD = 1.0 - iKS;
	iKD *= 1.0 - fMetallic;
	float3 irradiance = pow(SkyMap.Sample(splr, rotatedNormal).rgb, 2.2f);
	float3 iDiffuse = irradiance * albedo;

	const float MAX_REF_LOD = 4.0f;
	float3 prefilteredColor = pow(SkyMapMip.SampleLevel(splr, R, fRoughness * MAX_REF_LOD).rgb, 2.2f);
	float2 brdf = BRDFLUT.Sample(splrClamp, float2(NdotV, fRoughness)).rg;
	float3 iSpecular = prefilteredColor * (iKS * brdf.x + brdf.y);


	float ao = 1.0f;
	float3 ambient = (iKD * iDiffuse + iSpecular) * ao;

	float3 color = Light + ambient;

	color = color / (color + 1.0f);
	color = pow(color, 1.0f / 2.2f);



	//SkyMapMip.SampleLevel(splr, i.worldPos, roughness * MAX_REF_LOD).rgb
	return float4(color, 1.0f);
	//const float3 diffuse = PdiffuseColor * PdiffuseIntensity * att * max(0, dot(i.normal, PlightDir)) +
	//						DdiffuseColor * DdiffuseIntens6ity * max(0, dot(i.normal, direction));

	//const float3 PhalfDir = normalize(PlightDir + viewDir);

	//const float3 specular = PdiffuseColor * PdiffuseIntensity * att * specularIntensity * pow(max(0, dot(i.normal, PhalfDir)), specularPower) +
	//						DdiffuseColor * DdiffuseIntensity* specularIntensity * pow(max(0, dot(i.normal, DhalfDir)), specularPower);

	//return float4(ambient + diffuse * albedo + specular, 1.0);
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
		float textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * time * speed).r;
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
		float3 textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * time * speed).rgb;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}