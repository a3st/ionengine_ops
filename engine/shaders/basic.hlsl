/*
    HLSL was generated by shaderc v1.0
*/

struct WorldData {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct LightingData {
    float distance;
    float3 position;
    float3 direction;
    float3 color;
};

cbuffer SHADER_DATA : register(b0, space0) {
    uint materialData;
    uint linearSampler;
    uint worldData;
    uint lightingData;
};

struct VS_INPUT {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output;
    float4 world_pos = mul(ResourceDescriptorHeap[shaderData.worldData].model, float4(input.position, 1.0f));
    output.position = mul(ResourceDescriptorHeap[shaderData.worldData].projection, mul(ResourceDescriptorHeap[shaderData.worldData].view, world_pos));
    output.uv = float2(input.uv.x, input.uv.y);
    return output;
}

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT ps_main(VS_OUTPUT input) {
    PS_OUTPUT output;
    output.color = float4(0.2f, 0.3f, 0.1f, 1.0f);
    return output;
};