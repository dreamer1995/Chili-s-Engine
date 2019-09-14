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

cbuffer ObjectCBuf : register(b3)
{
    bool normalMapEnabled;
    float padding[3];
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
Texture2D spec;
Texture2D nmap;

SamplerState splr;


float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Binormal, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        // unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        // bring normal from tanspace into view space
        n = mul(n, tanToView);
    }
	// fragment to light vector data
	float3 vLightPos = mul(float4(lightPos, 1.0f), matrix_V).xyz;
	const float3 vToL = vLightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	const float3 dirView = mul(direction, (float3x3)matrix_V);
	// attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = PdiffuseColor * PdiffuseIntensity * att * max(0.0f, dot(dirToL, n)) +
							DdiffuseColor * DdiffuseIntensity * max(0.0f, dot(dirView, n));
	// reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
	const float3 _w = n * dot(dirView, n);
	const float3 _r = _w * 2.0f - dirView;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
	const float3 specular = att * (PdiffuseColor * PdiffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower) +
							DdiffuseColor * DdiffuseIntensity * pow(max(0.0f, dot(normalize(-_r), normalize(viewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), 1.0f);
}