cbuffer LightCBuf : register(b0)
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
	float specularIntensity;
	float specularPower;
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