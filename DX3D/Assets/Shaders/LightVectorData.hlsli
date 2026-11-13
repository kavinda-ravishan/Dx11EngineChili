struct LightVectorData
{
    float3 v_to_l;
    float3 dir_to_l;
    float dist_to_l;
};

LightVectorData CalculateLightVectorData(const in float3 light_pos, const in float3 frag_pos)
{
    LightVectorData lv;
    lv.v_to_l = light_pos - frag_pos;
    lv.dist_to_l = length(lv.v_to_l);
    lv.dir_to_l = lv.v_to_l / lv.dist_to_l;
    return lv;
}
