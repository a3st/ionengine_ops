// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Quad Shader";
    Domain = "Screen";
}

DATA {
    texture2D_t inputTexture;
}

VS {
    #include "shared/common.hlsli"

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
        return output;
    }
}

[FillMode("SOLID"), CullSide("BACK"), DepthWrite(true), StencilWrite(false)]
PS {
    #include "shared/common.hlsli"

    PS_OUTPUT main(VS_OUTPUT input) {
        cbuffer_t<SAMPLER_DATA> samplerBuffer = gShaderData.samplerBuffer;
        cbuffer_t<MATERIAL_DATA> materialBuffer = gShaderData.materialBuffer;

        texture2D_t inputTexture = materialBuffer.Get().inputTexture;
        sampler_t linearSampler = samplerBuffer.Get().linearSampler;

        PS_OUTPUT output;
        output.color = inputTexture.Get().Sample(linearSampler.Get(), input.uv);
        return output;
    }
}