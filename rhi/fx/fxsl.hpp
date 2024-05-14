// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rhi::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', 'S', 'L'};

    struct Header
    {
        std::array<uint8_t, Magic.size()> magic;
        uint32_t length;
        uint32_t target;
    };

    enum class ChunkType : uint32_t
    {
        Json,
        Binary
    };

    struct ChunkHeader
    {
        uint32_t chunkType;
        uint32_t chunkLength;
    };

    enum class ShaderElementType
    {
        Float4x4,
        Float3x3,
        Float2x2,
        Float4,
        Float3,
        Float2,
        Float,
        Uint,
        Bool,
        SamplerState,
        ConstantBuffer,
        StorageBuffer,
        Texture2D
    };

    enum class ShaderStageType
    {
        Vertex,
        Pixel,
        Compute
    };

    enum class ShaderTargetType : uint32_t
    {
        DXIL,
        SPIRV
    };

    enum class ShaderCullSide
    {
        Back,
        Front,
        None
    };

    struct ShaderConstantData
    {
        std::string name;
        ShaderElementType constantType;
        int32_t structure;
    };

    struct ShaderStructureElementData
    {
        std::string name;
        ShaderElementType elementType;
        std::string semantic;
    };

    struct ShaderStructureData
    {
        std::string name;
        std::vector<ShaderStructureElementData> elements;
        uint32_t size;
    };

    struct ShaderStageData
    {
        int32_t buffer;
        std::string entryPoint;
    };

    struct ShaderTechniqueData
    {
        std::unordered_map<std::string, ShaderStageData> stages;
        bool depthWrite;
        bool stencilWrite;
        ShaderCullSide cullSide;
    };

    struct ShaderEffect
    {
        ShaderTargetType target;
        ShaderTechniqueData technique;
        std::vector<ShaderConstantData> constants;
        std::vector<ShaderStructureData> structures;
        std::vector<std::vector<uint8_t>> buffers;
    };

    class FXSL
    {
      public:
        static auto loadFromMemory(std::span<uint8_t const> const data) -> std::optional<ShaderEffect>;

        static auto loadFromFile(std::filesystem::path const& filePath) -> std::optional<ShaderEffect>;

        static auto save(ShaderEffect const& object, std::filesystem::path const& filePath) -> bool;

        static auto save(ShaderEffect const& object, std::basic_stringstream<uint8_t>& stream) -> bool;

      private:
        static auto generateJsonChunkData(ShaderEffect const& object) -> std::string;

        static auto readJsonChunkData(ShaderEffect& object, std::vector<uint8_t>& data) -> bool;
    };
} // namespace ionengine::rhi::fx