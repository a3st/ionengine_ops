// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/asset.hpp"
#include "rhi/fx/fxsl.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;

    class ShaderAsset : public Asset
    {
      public:
        struct ShaderOption
        {
            uint32_t constantIndex;
            rhi::fx::ShaderElementType elementType;
            uint64_t offset;
            size_t size;
        };

        ShaderAsset(LinkedDevice& device);

        auto loadFromFile(std::filesystem::path const& filePath) -> bool;

        auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool;

        auto getOptions() const -> std::unordered_map<std::string, ShaderOption>;

        auto getShader() const -> core::ref_ptr<rhi::Shader>;

      private:
        LinkedDevice* device;
        core::ref_ptr<rhi::Shader> shaderProgram;
        std::unordered_map<std::string, ShaderOption> options;

        auto parseShaderEffectData(rhi::fx::ShaderEffectData const& shaderEffect) -> bool;
    };
} // namespace ionengine