// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    class Sampler2DNode : public Node
    {
      public:
        Sampler2DNode(uint64_t const nodeID, uint32_t const posX, uint32_t const posY)
            : Node(nodeID, "Sampler2D", posX, posY, {{"UV", "float2"}, {"Texture", "Texture2D"}}, {{"Color", "float4"}})
        {
        }

        auto generateInitialShaderCode() -> std::string override
        {
            return R"(
                [fx::shader_constant]
                SamplerState linearSampler;
            )";
        }

        auto generateResourceShaderCode() -> std::string override
        {
            return "";
        }

        auto generateRepeatShaderCode() -> std::string override
        {
            return R"(
                
            )";
        }
    };
} // namespace ionengine::tools::editor