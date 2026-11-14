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

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;

float4 main(float3 view_Frag_pos : Position, float3 view_normal : Normal, float3 view_tan : Tangent, float3 view_bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    view_normal = normalize(view_normal);
    // replace normal with mapped if normal mapping enabled
    if (normal_map_enabled)
    {
        view_normal = MapNormal(normalize(view_tan), normalize(view_bitan), view_normal, tc, nmap, splr);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_Frag_pos);
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular
    const float3 specular = Specular(
        diffuse_color, diffuse_intensity, view_normal,
        lv.v_to_l, view_Frag_pos, att, specular_power
    );
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}