#include <PhongHeader.hlsli>

Texture2D tex;
Texture2D nmap;

SamplerState splr;

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
		bumpNormal = nmap.Sample(splr, i.uv).xyz;
		bumpNormal = bumpNormal * 2.0f - 1.0f;
		bumpNormal = (bumpNormal.x * i.tangent) + (bumpNormal.y * i.binormal) + (bumpNormal.z * i.normal);
		i.normal = normalize(bumpNormal);
	}
	const float3 PlightDir = normalize(lightPos - i.worldPos);

	const float distToL = length(lightPos - i.worldPos);

	const float3 viewDir = normalize(cameraPos - i.worldPos);

	//float3 albedo = tex.Sample(splr, i.uv).rgb * color;
	const float3 albedo = tex.Sample(splr, i.uv).rgb;

	//fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * albedo;

	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

	const float3 diffuse = PdiffuseColor * PdiffuseIntensity * att * max(0, dot(i.normal, PlightDir)) +
							DdiffuseColor * DdiffuseIntensity * max(0, dot(i.normal, direction));

	const float3 PhalfDir = normalize(PlightDir + viewDir);
	const float3 DhalfDir = normalize(direction + viewDir);

	const float3 specular = PdiffuseColor * PdiffuseIntensity * att * specularIntensity * pow(max(0, dot(i.normal, PhalfDir)), specularPower) +
							DdiffuseColor * DdiffuseIntensity* specularIntensity * pow(max(0, dot(i.normal, DhalfDir)), specularPower);

	return float4(ambient + diffuse * albedo + specular, 1.0);
}