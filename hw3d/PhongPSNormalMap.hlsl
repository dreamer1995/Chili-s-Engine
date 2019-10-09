#include <PhongHeader.hlsli>

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;

float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Binormal, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if( normalMapEnabled )
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        // unpack the normal from map into tangent space        
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        n.y = -n.y;
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
	const float3 specular = att * (PdiffuseColor * PdiffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower) +
							DdiffuseColor * DdiffuseIntensity* specularIntensity * pow(max(0.0f, dot(normalize(-_r), normalize(viewPos))), specularPower);
	// final color
    //return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
	return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}