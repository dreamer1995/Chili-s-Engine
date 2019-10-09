#include <PBRHeader.hlsli>

//struct VertexToPixel
//{
//	float4 position		: SV_POSITION;
//	float2 uv           : TEXCOORD0;
//};

float2 main(float2 uv : Texcoord) : SV_TARGET
{
	
	float NdotV = uv.x;
	float roughness = uv.y;

	float3 viewDir;
	viewDir.x = sqrt(1.0f - (NdotV * NdotV)); // sin
	viewDir.y = 0.0f;
	viewDir.z = NdotV; // cos

	float A = 0.0f;
	float B = 0.0f;

	float3 normalVec = float3(0.0f, 0.0f, 1.0f);

	const uint numSamples = 1024;
	// generates a sample vector that's biased towards the
	// preferred alignment direction (importance sampling)
	for (uint i = 0; i < numSamples; i++)
	{
		float2 Xi = Hammersley(i, numSamples);
		float3 halfwayVec = ImportanceSampleGGX(Xi, roughness, normalVec);
		float3 lightDir = normalize((2.0f * dot(viewDir, halfwayVec) * halfwayVec) - viewDir);

		float NdotL = saturate(lightDir.z);
		float NdotH = saturate(halfwayVec.z);
		float VdotH = saturate(dot(viewDir, halfwayVec));

		if (NdotL > 0)
		{
			float G = IBLGeometrySmith(normalVec, viewDir, lightDir, roughness);
			float G_Vis = (G * VdotH) / (NdotH * NdotV);
			float Fc = pow(1.0f - VdotH, 5.0f);

			A += (1.0f - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	
	return float2(A, B) / numSamples;

	
}