float4 main(float2 uv : Texcoord) : SV_Position
{
	return float4(uv.x / 2.0f + 0.45f, uv.y / 2.0f - 0.1f, 0.0f, 1.0f);
}