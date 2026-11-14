#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specular_intensity;
    float specular_power;
    float padding[2];
};

Texture2D tex;

SamplerState splr;


float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal, float2 tc : Texcoord) : SV_Target
{
    // renormalize interpolated normal
    view_normal = normalize(view_normal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
	// specular
    const float3 specular = Specular(diffuse_color, diffuse_intensity, view_normal, lv.v_to_l, view_frag_pos, att, specular_power);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}