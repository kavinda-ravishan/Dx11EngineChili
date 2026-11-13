#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

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


float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal, float3 view_tan : Tangent, float3 view_bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    view_normal = normalize(view_normal);
    // replace normal with mapped if normal mapping enabled
    if (normal_map_enabled)
    {
        view_normal = MapNormal(normalize(view_tan), normalize(view_bitan), view_normal, tc, nmap, splr);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
    // specular parameter determination (mapped or uniform)
    float3 specular_reflection_color;
    float specular_power = specular_power_const;
    if (specular_map_enabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specular_reflection_color = specularSample.rgb * specular_map_weight;
        if (has_gloss)
        {
            specular_power = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specular_reflection_color = specular_color;
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