#include "Transform.hlsli"

struct VSOut
{
    float3 view_pos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 view_normal : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.view_pos = (float3) mul(float4(pos, 1.0f), model_view);
    vso.normal = mul(view_normal, (float3x3) model_view);
    vso.pos = mul(float4(pos, 1.0f), model_view_proj);
    vso.tc = tc;
    return vso;
}