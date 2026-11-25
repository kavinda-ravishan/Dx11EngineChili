#include "Inc/ShaderOps.hlsli"
#include "Inc/LightVectorData.hlsli"

#include "Inc/PointLight.hlsli"

cbuffer ObjectCBuf
{
    bool use_gloss_alpha;
    float3 specular_color;
    float specular_weight;
    float specular_gloss;
    float padding[2];
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
    float specular_power_loaded = specular_gloss;
    const float4 specular_sample = spec.Sample(splr, tc);
    const float3 specular_reflection_color = specular_sample.rgb;
    if (use_gloss_alpha)
    {
        specular_power_loaded = pow(2.0f, specular_sample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse light
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular reflected
    const float3 specular_reflected = Specular(
        diffuse_color * specular_reflection_color, specular_weight, view_normal,
        lv.v_to_l, view_frag_pos, att, specular_power_loaded
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular_reflected), 1.0f);
}