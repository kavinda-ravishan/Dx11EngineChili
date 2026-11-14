#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float4 material_color;
    float4 specular_color;
    float specular_power;
    float padding[3];
};

float4 main(float3 view_frag_pos : Position, float3 view_normal : Normal) : SV_Target
{
    // normalize the mesh normal
    view_normal = normalize(view_normal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_frag_pos);
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular
    const float3 specular = Specular(
        specular_color.rgb, 1.0f, view_normal,
        lv.v_to_l, view_frag_pos, att, specular_power
    );
	// final color
    return float4(saturate((diffuse + ambient) * material_color.rgb + specular), 1.0f);
}