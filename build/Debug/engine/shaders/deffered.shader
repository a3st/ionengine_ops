{
    name: "deffered_pc",
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            properties: [
                { name: "view", type: "float4x4" },
                { name: "proj", type: "float4x4" },
                { name: "camera_position", type: "float3" },
                { name: "inverse_view_proj", type: "float4x4" }
            ],
            visibility: "all"
        },
        {
            name: "light",
            type: "cbuffer",
            properties: [
                { name: "point_light_count", type: "_uint" }
            ],
            visibility: "pixel"
        },
        {
            name: "positions",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "albedo",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "normals",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "roughness_metalness_ao",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "point_light",
            type: "sbuffer",
            properties: [
                { name: "position", type: "float3" },
                { name: "range", type: "_float" },
                { name: "color", type: "float3" }
            ],
            visibility: "pixel"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "back",
        depth_stencil: false,
        blend_mode: "opaque"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "float3", semantic: "POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            source: "
                vs_output main(vs_input input) {
                    vs_output output;
                    output.position = float4(input.position, 1.0f);
                    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "color", type: "float4", semantic: "SV_TARGET0" }
            ],
            source: "

                #define IONENGINE_MATRIX_M objects[instance].model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj
                #define IONENGINE_MATRIX_INV_M objects[instance].inverse_model

                #define DIRECTIONAL_LIGHT_TYPE 0
                #define POINT_LIGHT_TYPE 1
                #define SPOT_LIGHT_TYPE 2

                static const float PI = 3.14159265359;

                float DistributionGGX(float3 N, float3 H, float roughness) {
                    float a = roughness * roughness;
                    float a2 = a * a;
                    float NdotH = max(dot(N, H), 0.0f);
                    float NdotH2 = NdotH * NdotH;

                    float num = a2;
                    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
                    denom = PI * denom * denom;

                    return num / max(denom, 0.0000001);
                }

                float GeometrySchlickGGX(float NdotV, float roughness) {
                    float r = (roughness + 1.0f);
                    float k = (r * r) / 8.0f;

                    float num   = NdotV;
                    float denom = NdotV * (1.0f - k) + k;

                    return num / denom;
                }

                float GeometrySmith(float3 N, float3 V, float3 L, float roughness) {
                    float NdotV = max(dot(N, V), 0.0f);
                    float NdotL = max(dot(N, L), 0.0f);
                    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
                    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

                    return ggx1 * ggx2;
                }

                float3 FresnelSchlick(float cosTheta, float3 F0) {
                    return F0 + (1.0 - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
                }

                float calculate_attenuation(float distance, float range) {
                    float att = saturate(1.0f - (distance * distance / (range * range)));
                    return att * att;
                }

                float3 calculate_point_light(point_light_data point_light, float3 position, float3 normal, float3 V, float3 albedo, float metalness, float roughness) {

                    float3 F0 = float3(0.04f, 0.04f, 0.04f);
                    F0 = lerp(F0, albedo, metalness);

                    float3 L = normalize(position - point_light.position);
                    float3 H = normalize(V + L);

                    // calculate per-light radiance
                    float distance = length(point_light.position - position);
                    float attenuation = calculate_attenuation(distance, point_light.range);
                    float3 radiance = point_light.color * attenuation;

                    // cook-torrance brdf
                    float NDF = DistributionGGX(normal, H, roughness);        
                    float G = GeometrySmith(normal, V, L, roughness);      
                    float3 F = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

                    float3 nominator = NDF * G * F;
                    float denominator = 4.0f * max(dot(normal, V), 0.0f) * max(dot(normal, L), 0.0f) + 0.0001f;
                    float3 specular = nominator / denominator;

                    // kS is equal to Fresnel
                    float3 kS = F;
                    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
                    kD *= 1.0 - metalness;

                    // scale light by NdotL
                    float NdotL = max(dot(normal, L), 0.0f);

                    // add to outgoing radiance Lo
                    float3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
                    return Lo;
                } 

                ps_output main(ps_input input) {
                    ps_output output;

                    float3 position = positions_texture.Sample(positions_sampler, input.uv).xyz;
                    float3 normal = normals_texture.Sample(normals_sampler, input.uv).xyz;

                    float3 V = normalize(position - world.camera_position);

                    float3 albedo = albedo_texture.Sample(albedo_sampler, input.uv).rgb;
                    float3 roughness_metalness_ao = roughness_metalness_ao_texture.Sample(roughness_metalness_ao_sampler, input.uv).rgb;
                    float roughness = roughness_metalness_ao.r;
                    float metalness = roughness_metalness_ao.g;
                    float ao = roughness_metalness_ao.b;

                    float3 Lo = float3(0.0f, 0.0f, 0.0f);

                    for(uint i = 0; i < light.point_light_count; ++i) {
                        Lo += calculate_point_light(point_lights[i], position, normal, V, albedo, metalness, roughness);
                    }
                    
                    //float3 ambient = float3(0.005f, 0.005f, 0.005f) * albedo;
                    //float3 color = Lo + ambient;
                    float3 color = pow(Lo, float3(1.4f / 2.2f, 1.4f / 2.2f, 1.4f / 2.2f));

                    output.color = float4(color, 1.0f);
                    return output;
                }
            "
        }
    ]
}
