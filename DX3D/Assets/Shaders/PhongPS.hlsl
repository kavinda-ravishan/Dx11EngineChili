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
    float3 material_color;
    float specular_intensity;
    float specular_power;
};

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
	
    // reflected light vector
    const float3 w = n * dot(v_to_l, n);
    const float3 r = w * 2.0f - v_to_l;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuse_color * diffuse_intensity) * specular_intensity * pow(max(0.0f, dot(normalize(-r), normalize(world_pos))), specular_power);
    
    // final color
    return float4(saturate((diffuse + ambient + specular) * material_color), 1.0f);
}