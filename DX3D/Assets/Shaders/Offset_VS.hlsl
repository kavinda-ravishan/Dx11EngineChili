#include "Inc/Transform.hlsli"

cbuffer Offset
{
    float offset;
}

float4 main(float3 pos : Position, float3 view_normal : Normal) : SV_Position
{
    return mul(float4(pos + view_normal * offset, 1.0f), model_view_proj);
}