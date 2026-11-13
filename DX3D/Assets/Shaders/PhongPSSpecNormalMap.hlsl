cbuffer LightCBuf
{
    float3 light_pos;
    float3 ambient;
    float3 diffuse_color;
    float diffuse_intensity;
    float att_const;
    float att_lin;
    float att_quad;
};

cbuffer ObjectCBuf
{
    bool normal_map_enabled;
    bool specular_map_enabled;
    bool has_gloss;
    float specular_power_const;
    float3 specular_color;
    float specular_map_weight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;

float3 MapNormalViewSpace(const float3 tan, const float3 bitan, const float3 view_normal, const float2 tc, Texture2D nmap, SamplerState splr)
{
    // build the tranform (rotation) into tangent space
    const float3x3 tan_to_view = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(view_normal)
        );
        
        // sample and unpack the normal from texture into tangent space
    const float3 normal_sample = nmap.Sample(splr, tc).xyz;
    float3 tan_normal;
    tan_normal = (normal_sample * 2.0f) - 1.0f;
    tan_normal.y = -tan_normal.y;
        // bring normal from tanspace into view space
    return normalize(mul(tan_normal, tan_to_view));
}

float4 main(float3 view_pos : Position, float3 view_normal : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    if (normal_map_enabled)
    {
        view_normal = MapNormalViewSpace(tan, bitan, view_normal, tc, nmap, splr);
    }
    
	// fragment to light vector data
    const float3 v_to_l = light_pos - view_pos;
    const float dist_to_l = length(v_to_l);
    const float3 dir_to_l = v_to_l / dist_to_l;
	
    // diffuse attenuation
    const float att = 1.0f / ((att_const) + (att_lin * dist_to_l) + (att_quad * dist_to_l * dist_to_l));
	// diffuse intensity
    const float3 diffuse = diffuse_color * diffuse_intensity * att * max(0.0f, dot(dir_to_l, view_normal));
	
    // reflected light vector
    const float3 w = view_normal * dot(v_to_l, view_normal);
    const float3 r = w * 2.0f - v_to_l;
	
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    float3 specular_reflection_color;
    float specular_power = specular_power_const;
    if(specular_map_enabled)
    {
        const float4 specular_sample = spec.Sample(splr, tc);
        specular_reflection_color = specular_sample.rgb * specular_map_weight;
        if(has_gloss)
        {
            specular_power = pow(2.0f, specular_sample.a * 13.0f);
        }
    }
    else
    {
        specular_reflection_color = specular_color;
    }
    
    const float3 specular = att * (diffuse_color * diffuse_intensity) * pow(max(0.0f, dot(normalize(-r), normalize(view_pos))), specular_power);
    
    // final color
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + (specular * specular_reflection_color), 1.0f);
}