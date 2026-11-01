cbuffer CBuf
{
    matrix model_view;
    matrix model_view_proj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 color : Color;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 color : Color)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model_view);
    vso.normal = mul(n, (float3x3) model_view);
    vso.pos = mul(float4(pos, 1.0f), model_view_proj);
    vso.color = color;
    return vso;
}