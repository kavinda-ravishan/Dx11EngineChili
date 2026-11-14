#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specular_power_const;
    bool has_gloss;
    float specular_map_weight;
};

Texture2D tex;
Texture2D spec;

SamplerState splr;

float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    view_normal = normalize(view_normal);
    
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
    
    // specular parameters
    float specular_power = specular_power_const;
    const float4 specular_sample = spec.Sample(splr, tc);
    const float3 specular_reflection_color = specular_sample.rgb * specular_map_weight;
    if (has_gloss)
    {
        specular_power = pow(2.0f, specular_sample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse light
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular reflected
    const float3 specular_reflected = Specular(
        specular_reflection_color, 1.0f, view_normal,
        lv.v_to_l, view_frag_pos, att, specular_power
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular_reflected), 1.0f);
}