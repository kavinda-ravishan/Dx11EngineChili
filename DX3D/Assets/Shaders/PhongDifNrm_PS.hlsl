#include "Inc/ShaderOps.hlsli"
#include "Inc/LightVectorData.hlsli"

#include "Inc/PointLight.hlsli"

cbuffer ObjectCBuf
{
    float3 specular_color;
    float specular_weight;
    float specular_gloss;
    bool use_normal_map;
    float normal_map_weight;
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
    if (use_normal_map)
    {
        const float3 mapped_normal = MapNormal(normalize(view_tan), normalize(view_bitan), view_normal, tc, nmap, splr);
        view_normal = lerp(view_normal, mapped_normal, normal_map_weight);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(view_light_pos, view_Frag_pos);
	// attenuation
    const float att = Attenuate(att_const, att_lin, att_quad, lv.dist_to_l);
	// diffuse
    const float3 diffuse = Diffuse(diffuse_color, diffuse_intensity, att, lv.dir_to_l, view_normal);
    // specular
    const float3 specular = Specular(
        diffuse_color * diffuse_intensity * specular_color, specular_weight, view_normal,
        lv.v_to_l, view_Frag_pos, att, specular_gloss
    );
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}