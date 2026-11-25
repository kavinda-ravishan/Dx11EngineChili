cbuffer CBuf : register(b1)
{
    float3 material_color;
    float padding[1];
};

float4 main() : SV_Target
{
    return float4(material_color, 1.0f);
}