cbuffer CBuf
{
    matrix model_view;
    matrix model_view_proj;
};

struct VSOut
{
    float3 view_pos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 view_normal : Normal)
{
    VSOut vso;
    vso.view_pos = (float3) mul(float4(pos, 1.0f), model_view);
    vso.normal = mul(view_normal, (float3x3) model_view);
    vso.pos = mul(float4(pos, 1.0f), model_view_proj);
    return vso;
}