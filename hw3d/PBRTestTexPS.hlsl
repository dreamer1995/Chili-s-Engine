cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 PdiffuseColor;
	float PdiffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer DirectionalLightCBuf : register(b1)
{
	float3 direction;
	float3 DdiffuseColor;
	float DdiffuseIntensity;
};

cbuffer CameraCBuf : register(b2)
{
	float3 cameraPos;
};

cbuffer ObjectCBuf : register(b3)
{
	float metallic;
	float _roughness;
	bool normalMapEnabled;
	float padding[1];
};

cbuffer TransformCBuf : register(b4)
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

Texture2D tex;
Texture2D nmap : register(t1);
Texture2D rmamap : register(t2);
TextureCube SkyMap : register(t3);
TextureCube SkyMapMip : register(t4);
Texture2D BRDFLUT : register(t5);

SamplerState splr;

struct PSIn {
	float3 worldPos : Position;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;
	float2 uv : Texcoord;
};

static float PI = 3.14159265359;

float DistributionGGX(float NdotH, float roughness);
float GeometrySchlickGGX(float dotedVector, float roughness);
float GeometrySmith(float NdotV, float NdotL, float roughness);
float3 FresnelSchlick(float NdotH, float3 F0);
float3 FresnelSchlickRoughness(float NdotV, float3 F0, float roughness);


float4 main(PSIn i) : SV_Target
{
	// sample normal from map if normal mapping enabled	
	if (normalMapEnabled)
	{
		float3 bumpNormal;
		bumpNormal = nmap.Sample(splr, i.uv).rgb;
		bumpNormal = bumpNormal * 2.0f - 1.0f;
		bumpNormal = (bumpNormal.x * i.tangent) + (bumpNormal.y * i.binormal) + (bumpNormal.z * i.normal);
		i.normal = normalize(bumpNormal);
	}
	//const float3 PlightDir = normalize(lightPos - i.worldPos);

	//const float distToL = length(lightPos - i.worldPos);

	const float3 viewDir = normalize(cameraPos - i.worldPos);
	//PBR Start
	const float NdotV = max(dot(i.normal, viewDir), 0.0f);
	const float NdotL = max(dot(i.normal, direction), 0.0f);
	const float3 halfDir = normalize(direction + viewDir);
	float NdotH = max(dot(i.normal, halfDir), 0.0f);
	float3 R = reflect(-viewDir, i.normal);
	//float3 albedo = tex.Sample(splr, i.uv).rgb * color;
	//float3(1.0f, 0.0f, 0.0f)
	
	float3 albedo = pow(tex.Sample(splr, i.uv).rgb, 2.2f);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	float fMetallic = metallic * rmamap.Sample(splr, i.uv).g;
	F0 = lerp(F0, albedo, fMetallic);
	//fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * albedo;

	//const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

	const float3 radiance = DdiffuseColor * DdiffuseIntensity;
	float fRoughness = _roughness * rmamap.Sample(splr, i.uv).r;

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
	float3 irradiance = pow(SkyMap.Sample(splr, i.normal).rgb, 2.2f);
	float3 iDiffuse = irradiance * albedo;

	const float MAX_REF_LOD = 4.0f;
	float3 prefilteredColor = pow(SkyMapMip.SampleLevel(splr, R, fRoughness * MAX_REF_LOD).rgb, 2.2f);
	float2 brdf = BRDFLUT.Sample(splr, float2(NdotV, fRoughness)).rg;
	float3 iSpecular = prefilteredColor * (iKS * brdf.x + brdf.y);


	float ao = rmamap.Sample(splr, i.uv).b;
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

float DistributionGGX(float NdotH, float roughness)
{
	float a2 = roughness * roughness;
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float _denominator = NdotH2 * (a2 - 1.0f) + 1.0f;
	float denominator = PI * _denominator*_denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float dotedVector, float roughness)
{
	float numerator = dotedVector;
	float denominator = dotedVector * (1.0f - roughness) + roughness;

	return numerator / denominator;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float HdotV, float3 F0)
{
	return F0 + (1.0f - F0)*pow(1.0 - HdotV, 5.0f);
}

float3 FresnelSchlickRoughness(float NdotV, float3 F0, float roughness)
{
	return F0 + (max((1.0f - roughness), F0) - F0)*pow(1.0 - NdotV, 5.0f);
}