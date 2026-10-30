cbuffer LightCBuf
{
    float3 light_pos;
};

static const float3 material_color = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.05f, 0.05f, 0.05f };
static const float3 diffuse_color = { 1.0f, 1.0f, 1.0f };
static const float diffuse_intensity = 1.0f;
static const float att_const = 1.0f;
static const float att_lin = 0.045f;
static const float att_quad = 0.0075f;

float4 main(float3 world_pos : Position, float3 n : Normal) : SV_Target
{
	// fragment to light vector data
    const float3 v_to_l = light_pos - world_pos;
    const float dist_to_l = length(v_to_l);
    const float3 dir_to_l = v_to_l / dist_to_l;
	// diffuse attenuation
    const float att = 1.0f / ((att_const) + (att_lin * dist_to_l) + (att_quad * dist_to_l * dist_to_l));
	// diffuse intensity
    const float3 diffuse = diffuse_color * diffuse_intensity * att * max(0.0f, dot(dir_to_l, n));
	// final color
    return float4(saturate(diffuse + ambient), 1.0f);
}