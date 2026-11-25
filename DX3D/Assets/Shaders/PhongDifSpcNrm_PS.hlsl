#include "Inc/ShaderOps.hlsli"
#include "Inc/LightVectorData.hlsli"

#include "Inc/PointLight.hlsli"

cbuffer ObjectCBuf
{
    bool use_gloss_alpha;
    float specular_weight;
    float specular_gloss;
    bool use_normal_map;
    float normal_map_weight;
    float padding[3];
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;


float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal, float3 view_tan : Tangent, float3 view_bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample diffuse texture
    float4 dtex = tex.Sample(splr, tc);
    
#ifdef MASK
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // flip normal when backface
    if(dot(view_normal, view_frag_pos) >= 0.0f)
    {
        view_normal = -view_normal;
    }
#endif // MASK

    // normalize the mesh normal
    view_normal = normalize(view_normal);
    
    // replace normal with mapped if normal mapping enabled
    if (use_normal_map)
    {
        view_normal = MapNormal(normalize(view_tan), normalize(view_bitan), view_normal, tc, nmap, splr);
    }
	
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
    
    // specular parameter determination (mapped or uniform)
    float3 specular_reflection_color;
    float specular_power = specular_gloss;
    const float4 specular_sample = spec.Sample(splr, tc);
    specular_reflection_color = specular_sample.rgb;
    if (use_gloss_alpha)
    {
        specular_power = pow(2.0f, specular_sample.a * 13.0f);
    }

	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse light
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular reflected
    const float3 specular_reflected = Specular(
        diffuse_color * diffuse_intensity * specular_reflection_color, specular_weight, view_normal,
        lv.v_to_l, view_frag_pos, att, specular_power
    );
    
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * dtex.rgb + specular_reflected), dtex.a);
}