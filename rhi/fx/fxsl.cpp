// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "fxsl.hpp"
#include "core/exception.hpp"
#include "precompiled.h"
#include <simdjson.h>

namespace ionengine::rhi::fx
{
    template <typename Type>
    auto toString(Type const type) -> std::string;

    template <>
    auto toString(ShaderCullSide const type) -> std::string
    {
        switch (type)
        {
            case ShaderCullSide::None: {
                return "none";
            }
            case ShaderCullSide::Back: {
                return "back";
            }
            case ShaderCullSide::Front: {
                return "front";
            }
        }

        throw std::invalid_argument("Passed invalid argument into function");
    }

    template <>
    auto toString(ShaderElementType const type) -> std::string
    {
        switch (type)
        {
            case ShaderElementType::Bool: {
                return "BOOL";
            }
            case ShaderElementType::ConstantBuffer: {
                return "CONST_BUFFER";
            }
            case ShaderElementType::Float2: {
                return "FLOAT2";
            }
            case ShaderElementType::Float2x2: {
                return "FLOAT2x2";
            }
            case ShaderElementType::Float3: {
                return "FLOAT3";
            }
            case ShaderElementType::Float3x3: {
                return "FLOAT3x3";
            }
            case ShaderElementType::Float4: {
                return "FLOAT4";
            }
            case ShaderElementType::Float4x4: {
                return "FLOAT4x4";
            }
            case ShaderElementType::Float: {
                return "FLOAT";
            }
            case ShaderElementType::SamplerState: {
                return "SAMPLER_STATE";
            }
            case ShaderElementType::StorageBuffer: {
                return "STORAGE_BUFFER";
            }
            case ShaderElementType::Texture2D: {
                return "TEXTURE_2D";
            }
            case ShaderElementType::Uint: {
                return "UINT";
            }
        }

        throw std::invalid_argument("Passed invalid argument into function");
    }

    auto fromString(std::string_view const type) -> ShaderElementType;

    auto FXSL::loadFromMemory(std::span<uint8_t const> const data) -> std::optional<ShaderEffect>
    {
        return std::nullopt;
    }

    auto FXSL::loadFromFile(std::filesystem::path const& filePath) -> std::optional<ShaderEffect>
    {
        return std::nullopt;
    }

    auto FXSL::save(ShaderEffect& object, std::filesystem::path const& filePath) -> bool
    {
        std::stringstream json(std::ios::out);
        json << "{\"technique\":{";
        for (auto const& [stageName, stageInfo] : object.technique.stages)
        {
            json << "\"" + stageName + "\":{\"buffer\":" + std::to_string(stageInfo.buffer) + ",\"entryPoint\":\"" +
                        stageInfo.entryPoint + "\"},";
        }

        json << "\"depthWrite\":" << std::boolalpha << object.technique.depthWrite
             << ",\"stencilWrite\":" << std::boolalpha << object.technique.stencilWrite << ",\"cullSide\":"
             << "\"" + toString(object.technique.cullSide) + "\"},\"constants\":[";

        bool isFirst = true;

        for (auto const& constant : object.constants)
        {
            if (!isFirst)
            {
                json << ",";
            }

            json << "{\"name\":\"" + constant.name + "\",\"type\":\"" + toString(constant.constantType) +
                        "\",\"structure\":" + std::to_string(constant.structure) + "}";

            isFirst = false;
        }
        json << "],";

        std::cout << json.str() << std::endl;

        std::basic_stringstream<uint8_t> stream(std::ios::binary | std::ios::out);
        return false;
    }

} // namespace ionengine::rhi::fx

/*
namespace ionengine::rhi::shaderfile
{
    auto get_element_type_by_string(std::string_view const element_type) -> ElementType
    {
        ElementType ret{};
        if (element_type.compare("FLOAT4x4") == 0)
        {
            ret = ElementType::Float4x4;
        }
        else if (element_type.compare("FLOAT3x3") == 0)
        {
            ret = ElementType::Float3x3;
        }
        else if (element_type.compare("FLOAT2x2") == 0)
        {
            ret = ElementType::Float2x2;
        }
        else if (element_type.compare("FLOAT4") == 0)
        {
            ret = ElementType::Float4;
        }
        else if (element_type.compare("FLOAT3") == 0)
        {
            ret = ElementType::Float3;
        }
        else if (element_type.compare("FLOAT2") == 0)
        {
            ret = ElementType::Float2;
        }
        else if (element_type.compare("FLOAT") == 0)
        {
            ret = ElementType::Float;
        }
        else if (element_type.compare("UINT") == 0)
        {
            ret = ElementType::Uint;
        }
        else if (element_type.compare("BOOL") == 0)
        {
            ret = ElementType::Bool;
        }
        return ret;
    }

    auto get_resource_type_by_string(std::string_view const resource_type) -> ResourceType
    {
        ResourceType ret{};
        if (resource_type.compare("UNIFORM") == 0)
        {
            ret = ResourceType::Uniform;
        }
        else if (resource_type.compare("NON_UNIFORM") == 0)
        {
            ret = ResourceType::NonUniform;
        }
        return ret;
    }

    auto get_shader_stage_type_by_string(std::string_view const stage_type) -> ShaderStageType
    {
        ShaderStageType ret{};
        if (stage_type.compare("VERTEX_SHADER") == 0)
        {
            ret = ShaderStageType::Vertex;
        }
        else if (stage_type.compare("PIXEL_SHADER") == 0)
        {
            ret = ShaderStageType::Pixel;
        }
        return ret;
    }

    ShaderFile::ShaderFile(std::span<uint8_t const> const data)
    {
        std::basic_ispanstream<uint8_t> stream(std::span<uint8_t>(const_cast<uint8_t*>(data.data()), data.size()),
                                               std::ios::binary);

        Header header;
        stream.read((uint8_t*)&header, sizeof(Header));

        if (std::memcmp(header.magic.data(), SHADER_MAGIC.data(), sizeof(SHADER_MAGIC)) != 0)
        {
            throw core::Exception("Trying to parse a corrupted shader");
        }

        flags = static_cast<ShaderFileFlags>(header.flags);

        std::vector<uint8_t> chunk_json_data;
        {
            ChunkHeader chunk_header;
            stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

            chunk_json_data.resize(chunk_header.chunk_length);
            stream.read(chunk_json_data.data(), chunk_json_data.size());
        }

        simdjson::ondemand::parser parser;
        auto document = parser.iterate(chunk_json_data.data(), chunk_json_data.size(),
                                       chunk_json_data.size() + simdjson::SIMDJSON_PADDING);

        shader_name = document["shaderName"].get_string().value();

        for (auto stage : document["stages"].get_object())
        {
            std::vector<VertexStageInput> inputs;
            uint32_t inputs_size_per_vertex = 0;

            ShaderStageType stage_type = get_shader_stage_type_by_string(stage.unescaped_key().value());
            if (stage_type == ShaderStageType::Vertex)
            {
                for (auto input : stage.value()["inputs"])
                {
                    auto input_data =
                        VertexStageInput{.element_type = get_element_type_by_string(input["type"].get_string().value()),
                                         .semantic = std::string(input["semantic"].get_string().value())};
                    inputs.emplace_back(input_data);
                }
                inputs_size_per_vertex = static_cast<uint32_t>(stage.value()["inputsSizePerVertex"].get_uint64());
            }

            auto stage_data =
                ShaderStageData{.buffer = static_cast<uint32_t>(stage.value()["buffer"].get_uint64()),
                                .entry_point = std::string(stage.value()["entryPoint"].get_string().value()),
                                .inputs = std::move(inputs),
                                .inputs_size_per_vertex = inputs_size_per_vertex};

            stages.emplace(stage_type, stage_data);
        }

        for (auto resource : document["exports"].get_object())
        {
            std::vector<ElementData> elements;
            uint32_t size = 0;

            ResourceType resource_type = get_resource_type_by_string(resource.value()["type"].get_string().value());

            if (resource_type == ResourceType::Uniform)
            {
                for (auto element : resource.value()["elements"])
                {
                    auto input_data =
                        ElementData{.name = std::string(element["name"].get_string().value()),
                                    .element_type = get_element_type_by_string(element["type"].get_string().value()),
                                    .offset = element["offset"].get_uint64().value()};
                    elements.emplace_back(input_data);
                }
                size = static_cast<uint32_t>(resource.value()["sizeInBytes"].get_uint64());
            }

            auto resource_data =
                ResourceData{.binding = static_cast<uint32_t>(resource.value()["binding"].get_uint64()),
                             .resource_type = resource_type,
                             .elements = std::move(elements),
                             .size = size};

            resources.emplace(resource.unescaped_key().value(), resource_data);
        }

        do
        {
            std::vector<uint8_t> chunk_buffer_data;
            {
                ChunkHeader chunk_header;
                stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

                chunk_buffer_data.resize(chunk_header.chunk_length);
                stream.read(chunk_buffer_data.data(), chunk_buffer_data.size());
            }
            buffers.emplace_back(chunk_buffer_data);
        } while (header.length > stream.tellg());
    }

    auto ShaderFile::get_name() const -> std::string_view
    {
        return shader_name;
    }

    auto ShaderFile::get_resources() const -> std::unordered_map<std::string, ResourceData> const&
    {
        return resources;
    }

    auto ShaderFile::get_stages() const -> std::unordered_map<ShaderStageType, ShaderStageData> const&
    {
        return stages;
    }

    auto ShaderFile::get_buffer(uint32_t const index) const -> std::span<uint8_t const>
    {
        return buffers[index];
    }

    auto ShaderFile::get_flags() const -> ShaderFileFlags
    {
        return flags;
    }
} // namespace ionengine::rhi::shaderfile

*/