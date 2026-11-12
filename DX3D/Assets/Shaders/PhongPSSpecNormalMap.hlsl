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
    bool has_gloss;
    float specular_power_const;
    float padding[1];
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;

float4 main(float3 view_pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    if (normal_map_enabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tan_to_view = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        
        const float3 normal_sample = nmap.Sample(splr, tc).xyz;
        n = normal_sample * 2.0f - 1.0f;
        
        // bring normal from tanspace into view space
        n = mul(n, tan_to_view);
    }
    
	// fragment to light vector data
    const float3 v_to_l = light_pos - view_pos;
    const float dist_to_l = length(v_to_l);
    const float3 dir_to_l = v_to_l / dist_to_l;
	
    // diffuse attenuation
    const float att = 1.0f / ((att_const) + (att_lin * dist_to_l) + (att_quad * dist_to_l * dist_to_l));
	// diffuse intensity
    const float3 diffuse = diffuse_color * diffuse_intensity * att * max(0.0f, dot(dir_to_l, n));
	
    // reflected light vector
    const float3 w = n * dot(v_to_l, n);
    const float3 r = w * 2.0f - v_to_l;
	
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specular_sample = spec.Sample(splr, tc);
    
    const float3 specular_reflection_color = specular_sample.rgb;
    
    float3 specular_power;
    if(has_gloss)
    {
        specular_power = pow(2.0f, specular_sample.a * 13.0f);
    }
    else
    {
        specular_power = specular_power_const;
    }
    
    const float3 specular = att * pow(max(0.0f, dot(normalize(-r), normalize(view_pos))), specular_power);
    
    // final color
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + (specular * specular_reflection_color), 1.0f);
}