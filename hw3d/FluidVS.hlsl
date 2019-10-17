cbuffer CBuf
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

cbuffer CameraCBuf : register(b1)
{
	float3 cameraPos;
	float3 cameraDir;
};

cbuffer ObjectCBuf : register(b2)
{
	float3 color;
	float3 attenuation;
	float3 scatteringKd;
	float depth;
};

struct VSOut
{
	float3 worldPos : Position;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 bitangent : Binormal;
	float2 uv : Texcoord;
	float3 outScattering : Position1;
	float3 inScattering : Position2;
	float4 pos : SV_Position;
};

Texture2D hmap : register(t30);
SamplerState splr;

struct VSIn {
	float3 pos : Position;
	float3 n : Normal;
	float3 t : Tangent;
	float3 b : Binormal;
	float2 uv : Texcoord;
};

VSOut main(VSIn v)
{
	VSOut o;

	o.pos = mul(float4(v.pos, 1.0f), matrix_MVP);
	o.worldPos = (float3) mul(float4(v.pos, 1.0f), matrix_M2W);
	o.normal = normalize(mul(v.n, (float3x3)matrix_M2W));
	o.tangent = normalize(mul(v.t, (float3x3)matrix_M2W));
	o.bitangent = normalize(mul(v.b, (float3x3)matrix_M2W));
	o.uv = v.uv;

	const float3 viewDir = normalize(cameraPos - o.worldPos);

	float depthmap = hmap.SampleLevel(splr, v.uv, 0.0f).r;

	float t = depth * depthmap / (cameraPos.y - o.worldPos.y);

	//Water scattering

	float d = length(viewDir) * t;  // one way!

	o.outScattering = exp(-attenuation * d);

	o.inScattering = color * (1 - o.outScattering * exp(-depth * depthmap * scatteringKd));

	return o;
}