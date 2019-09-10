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

struct pin
{
	float3 worldPos : Position;
	float3 n : Normal;
	float2 tc : Texcoord;
};

float4 main(pin i) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if( normalMapEnabled )
    {
        const float3 normalSample = nmap.Sample(splr, i.tc).xyz;
        i.n.x = normalSample.x * 2.0f - 1.0f;
        i.n.y = -normalSample.y * 2.0f + 1.0f;
        i.n.z = -normalSample.z;
        i.n = mul(i.n, (float3x3)matrix_MV);
    }
	// fragment to light vector data
    const float3 vToL = lightPos - i.worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, i.n));
	// reflected light vector
    const float3 w = i.n * dot(vToL, i.n);
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(i.worldPos))), specularPower);
	// final color
    //return float4(saturate((diffuse + ambient) * tex.Sample(splr, i.tc).rgb + specular), 1.0f);
	return float4(saturate((diffuse + ambient) * tex.Sample(splr, i.tc).rgb + specular), 1.0f);
}