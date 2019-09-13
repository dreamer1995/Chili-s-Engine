cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer CameraCBuf : register(b1)
{
	float3 cameraPos;
};

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

cbuffer TransformCBuf
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
		i.normal = bumpNormal;
	}
	const float3 lightDir = normalize(lightPos - i.worldPos);

	const float distToL = length(lightPos - i.worldPos);

	const float3 viewDir = normalize(cameraPos - i.worldPos);

	//float3 albedo = tex.Sample(splr, i.uv).rgb * color;
	const float3 albedo = tex.Sample(splr, i.uv).rgb;

	//fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * albedo;

	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

	const float3 diffuse = albedo * diffuseColor * diffuseIntensity * att * max(0, dot(i.normal, lightDir));

	const float3 halfDir = normalize(lightDir + viewDir);

	const float3 specular = diffuseColor * diffuseIntensity * att * specularIntensity * pow(max(0, dot(i.normal, halfDir)), specularPower);

	return float4(ambient + diffuse + specular, 1.0);

	//// fragment to light vector data
	//float3 vLightPos = mul(float4(lightPos,1.0f), matrix_V).xyz;
	//const float3 vToL = vLightPos - i.worldPos;
	//const float distToL = length(vToL);
	//const float3 dirToL = vToL / distToL;
	//// attenuation
	//
	//// diffuse intensity
	//const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, i.normal));
	//// reflected light vector
	//const float3 w = i.normal * dot(vToL, i.normal);
	//const float3 r = w * 2.0f - vToL;
	//// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	//const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(i.worldPos))), specularPower);
	//// final color
	//return float4(saturate((diffuse + ambient) * tex.Sample(splr, i.uv).rgb + specular), 1.0f);

}