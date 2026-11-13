cbuffer LightCBuf
{
    float3 view_light_pos;
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

float3 MapNormal(
    const in float3 tan, 
    const in float3 bitan, 
    const in float3 view_normal, 
    const in float2 tc, 
    uniform Texture2D nmap, 
    uniform SamplerState splr)
{
    // build the tranform (rotation) into same space as tan/bitan/normal (target space)
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

float Attenuate(uniform float att_const, uniform float att_lin, uniform float att_quad, const in float dist_frag_to_l)
{
    return 1.0f / ((att_const) + (att_lin * dist_frag_to_l) + (att_quad * dist_frag_to_l * dist_frag_to_l));
}

float3 Diffuse(
    uniform float3 diffuse_color,
    uniform float diffuse_intensity,
    const in float att,
    const in float3 view_dir_frag_to_l,
    const in float3 view_normal)
{
    return diffuse_color * diffuse_intensity * att * max(0.0f, dot(view_dir_frag_to_l, view_normal));
}

float3 Specular(
    const in float3 specular_color,
    uniform float specular_intensity,
    const in float3 view_normal,
    const in float3 view_frag_to_l,
    const in float3 view_pos,
    const in float att,
    const in float specular_power)
{
    // calculate reflected light vector
    const float3 w = view_normal * dot(view_frag_to_l, view_normal);
    const float3 r = normalize((w * 2.0f) - view_frag_to_l);
    
    // vector from camera to fragment (in view space)
    const float3 view_cam_to_frag = normalize(view_pos);
    
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return att * (diffuse_color * diffuse_intensity) * pow(max(0.0f, dot(-r, view_cam_to_frag)), specular_power);
}

float4 main(float3 view_pos : Position, float3 view_normal : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    if (normal_map_enabled)
    {
        view_normal = MapNormal(tan, bitan, view_normal, tc, nmap, splr);
    }
    
	// fragment to light vector data
    const float3 view_frag_to_l = view_light_pos - view_pos;
    const float dist_frag_to_l = length(view_frag_to_l);
    const float3 view_dir_frag_to_l = view_frag_to_l / dist_frag_to_l;
	
    // attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, dist_frag_to_l);
	
    // diffuse light
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, view_dir_frag_to_l, view_normal);
	
    // specular parameter determination (mapped or uniform)
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
    
    const float3 specular_reflected = Specular(specular_reflection_color, 1.0f, view_normal, view_frag_to_l, view_pos, att, specular_power);
    
    // final color attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular_reflected), 1.0f);
}