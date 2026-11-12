cbuffer CBuf
{
    matrix model_view;
    matrix model_view_proj;
};

float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), model_view_proj);
}