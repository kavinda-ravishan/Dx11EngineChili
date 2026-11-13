float3 MapNormal(
    const in float3 tan,
    const in float3 bitan,
    const in float3 normal,
    const in float2 tc,
    uniform Texture2D nmap,
    uniform SamplerState splr)
{
    // build the tranform (rotation) into same space as tan/bitan/normal (target space)
    const float3x3 tan_to_target = float3x3(tan, bitan, normal);
    // sample and unpack the normal from texture into tangent space
    const float3 normal_sample = nmap.Sample(splr, tc).xyz;
    const float3 tan_normal = (normal_sample * 2.0f) - 1.0f;
    // bring normal from tanspace into view space
    return normalize(mul(tan_normal, tan_to_target));
}

float Attenuate(uniform float att_const, uniform float att_lin, uniform float att_quad, const in float dist_frag_to_l)
{
    return 1.0f / ((att_const) + (att_lin * dist_frag_to_l) + (att_quad * dist_frag_to_l * dist_frag_to_l));
}

float3 Diffuse(
    uniform float3 diffuse_color,
    uniform float diffuse_intensity,
    const in float att,
    const in float3 view_dir_frag_to_l,
    const in float3 view_normal)
{
    return diffuse_color * diffuse_intensity * att * max(0.0f, dot(view_dir_frag_to_l, view_normal));
}

float3 Specular(
    const in float3 specular_color,
    uniform float specular_intensity,
    const in float3 view_normal,
    const in float3 view_frag_to_l,
    const in float3 view_pos,
    const in float att,
    const in float specular_power)
{
    // calculate reflected light vector
    const float3 w = view_normal * dot(view_frag_to_l, view_normal);
    const float3 r = normalize((w * 2.0f) - view_frag_to_l);
    
    // vector from camera to fragment (in view space)
    const float3 view_cam_to_frag = normalize(view_pos);
    
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return att * (specular_color * specular_intensity) * pow(max(0.0f, dot(-r, view_cam_to_frag)), specular_power);
}