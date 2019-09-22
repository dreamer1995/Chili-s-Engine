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
	float3 color;
    float specularIntensity;
    float specularPower;
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

SamplerState splr;

struct PSIn {
	float3 worldPos : Position;
	float3 normal : Normal;
};

float4 main(PSIn i) : SV_Target
{
	const float3 viewDir = normalize(cameraPos - i.worldPos);

	float3 albedo = color;

	const float3 diffuse = albedo * DdiffuseColor * DdiffuseIntensity * 0.1f * max(0, dot(i.normal, direction));

	const float3 halfDir = normalize(direction + viewDir);

	const float3 specular = DdiffuseColor * DdiffuseIntensity * 0.1f * specularIntensity * pow(max(0, dot(i.normal, halfDir)), specularPower);

	return float4(diffuse + specular, 1.0);
}