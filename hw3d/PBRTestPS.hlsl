#include <PBRHeader.hlsli>

Texture2D tex;
Texture2D nmap : register(t1);

struct PSIn {
	float3 worldPos : Position;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;
	float2 uv : Texcoord;
};

float4 main(PSIn i) : SV_Target
{
	// sample normal from map if normal mapping enabled	
	if (normalMapEnabled)
	{
		float3 bumpNormal;
		bumpNormal = nmap.Sample(splr, i.uv).rgb;
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
	
	float3 albedo = pow(float3(1.0f, 1.0f, 1.0f), 2.2f);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);
	//fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * albedo;

	//const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

	const float3 radiance = DdiffuseColor * DdiffuseIntensity;

	float NDF = DistributionGGX(NdotH, _roughness);
	float G = GeometrySmith(NdotV, NdotL, _roughness);
	float3 F = FresnelSchlick(max(dot(halfDir, viewDir), 0.0f), F0);

	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0f * NdotV * NdotL + 0.001f;
	float3 specular = numerator / denominator;

	float3 Light = (kD * albedo / PI + specular) * radiance * NdotL;

	float3 iKS = FresnelSchlickRoughness(NdotV, F0, _roughness);
	float3 iKD = 1.0 - iKS;
	iKD *= 1.0 - metallic;
	float3 irradiance = pow(SkyMap.Sample(splr, rotatedNormal).rgb, 2.2f);
	float3 iDiffuse = irradiance * albedo;

	const float MAX_REF_LOD = 4.0f;
	float3 prefilteredColor = pow(SkyMapMip.SampleLevel(splr, R, _roughness * MAX_REF_LOD).rgb, 2.2f);
	float2 brdf = BRDFLUT.Sample(splrClamp, float2(NdotV, _roughness)).rg;
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