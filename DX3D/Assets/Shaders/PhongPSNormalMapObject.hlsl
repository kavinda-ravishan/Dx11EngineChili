#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specular_intensity;
    float specular_power;
    bool normal_map_enabled;
    float padding[1];
};

#include "Transform.hlsli"

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normal_map_enabled)
    {
        // sample and unpack normal data
        const float3 normal_sample = nmap.Sample(splr, tc).xyz;
        const float3 object_normal = normal_sample * 2.0f - 1.0f;
        // bring normal from object space into view space
        view_normal = normalize(mul(object_normal, (float3x3) model_view));
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
	// specular
    const float3 specular = Specular(
        specular_intensity.rrr, 1.0f, view_normal, lv.v_to_l,
        view_frag_pos, att, specular_power
    );
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}