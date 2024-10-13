//TEXTURES & SAMPLER STATE
Texture2D testTexture :register(t0);
SamplerState testSampler;

//CONSTANT BUFFER OBJECTS
cbuffer LIGHT : register(b0)
{
	float4	light_pos;
	float4	light_color;
	float3	view_pos;
};

cbuffer MATERIAL : register(b1)
{
	float4 m_ambi; //ambient light color
	float4 m_diff; //diffuse light color
	float4 m_spec; //specular light color
	float  m_sfac; //specular factor
}

//VERTEX SHADER OUTPUT
struct PS_IN
{
	float4 vpos : SV_POSITION;
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

float4 main(PS_IN input) : SV_TARGET
{

	// INITIALISE VALUES
	float4 final_pixel = (float4)1; //return value, initialized to 0, 0, 0, 0
	// SAMPLE TEXTURE
	float3 tex = testTexture.Sample(testSampler, input.uv).xyz;

	// AMBIENT
	float3 ambient_lighting = light_color.xyz * m_ambi.xyz;

	// DIFFUSE CALCULATIONS
	float3 normal = normalize(input.normal);
	float3 light_dir = normalize(input.pos - light_pos.xyz); //directional vector from light to surface
	float  diffuse_factor = max(dot(normal, -light_dir), 0); //cosine of angle between the light and surface normal
	float3 diffuse_lighting = light_color.xyz * diffuse_factor * m_diff.xyz;

	// SPECULAR CALCULATIONS
	float3 view_dir = normalize(view_pos - input.pos);
	float3 reflection = reflect(light_dir, normal); // v = i - 2 * n * dot(i n) .
	float  spec = pow(max(dot(view_dir, reflection), 0.0), m_sfac);
	float3 specular_lighting = light_color.xyz * spec * m_spec.xyz;

	// APPLY COLORS
	final_pixel.xyz = (ambient_lighting + diffuse_lighting) * tex.xyz + specular_lighting;
	return final_pixel;
}