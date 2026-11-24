cbuffer CBuf : register(b1)
{
    float3 color;
    float padding[1];
};

float4 main() : SV_Target
{
    return float4(color, 1.0f);
}