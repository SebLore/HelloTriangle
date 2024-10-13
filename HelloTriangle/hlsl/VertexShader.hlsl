
cbuffer MATRIX : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 project;
};

struct VS_IN 
{
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 vpos : SV_POSITION;
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4x4 transform = mul(world, mul(view, project));

	output.vpos   = mul(float4(input.pos, 1.0f), transform);
	output.pos	  = mul(float4(input.pos, 1.0f), world).xyz; //avoid truncation
	output.normal = mul(float4(input.normal, 1.0f), world).xyz;
	output.uv = input.uv;
	return output;
}