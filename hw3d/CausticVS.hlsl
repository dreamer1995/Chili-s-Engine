struct VSOut
{
	float3 pos : position;
	float2 uv : Texcoord;
};

VSOut main(float3 pos : Position, float2 uv : Texcoord)
{
	VSOut o;
	o.pos = pos;
	o.uv = uv;
	return o;
}