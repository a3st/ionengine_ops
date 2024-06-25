// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serializable.hpp"
#include "engine/engine.hpp"
#include "generator.hpp"
#include "graph/scene.hpp"

namespace ionengine::tools::editor
{
    std::array<uint8_t, 8> constexpr ShaderGraphFileType{'S', 'G', 'F', 'I', 'L', 'E', '\0', '\0'};

    enum class ShaderGraphType : uint32_t
    {
        Lit,
        Unlit
    };

    struct NodeSocketData
    {
        std::string socketName;
        std::string socketType;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(socketName, "name");
            archive.property(socketType, "type");
        }
    };

    struct NodeUserData
    {
        uint32_t nodeComponentID;
        std::unordered_map<std::string, std::string> nodeOptions;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(nodeComponentID, "componentID");
            archive.property(nodeOptions, "options");
        }
    };

    struct NodeData
    {
        uint64_t nodeID;
        std::string nodeName;
        std::array<int32_t, 2> nodePosition;
        std::vector<NodeSocketData> nodeInputs;
        std::vector<NodeSocketData> nodeOutputs;
        NodeUserData nodeUserData;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(nodeID, "id");
            archive.property(nodePosition, "position");
            archive.property(nodeInputs, "inputs");
            archive.property(nodeOutputs, "outputs");
            archive.property(nodeUserData, "userData");
        }
    };

    struct ConnectionData
    {
        uint64_t connectionID;
        uint64_t sourceNodeID;
        uint32_t sourceIndex;
        uint64_t destNodeID;
        uint32_t destIndex;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(connectionID, "id");
            archive.property(sourceNodeID, "source");
            archive.property(sourceIndex, "out");
            archive.property(destNodeID, "dest");
            archive.property(destIndex, "in");
        }
    };

    struct SceneData
    {
        std::vector<NodeData> nodes;
        std::vector<ConnectionData> connections;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(nodes, "nodes");
            archive.property(connections, "connections");
        }
    };

    struct ShaderGraphData
    {
        ShaderGraphType graphType;
        SceneData sceneData;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(graphType, "graphType");
            archive.property(sceneData, "sceneData");
        }
    };

    struct ShaderGraphFile
    {
        asset::Header fileHeader;
        ShaderGraphData graphData;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(fileHeader);
            archive.with<core::serialize::OutputJSON, core::serialize::InputJSON>(graphData);
        }
    };

    struct ShaderGraphResult : public core::ref_counted_object
    {
        std::vector<uint8_t> outputShaderData;
    };

    class ShaderGraphEditor
    {
      public:
        ShaderGraphEditor();

        auto create(ShaderGraphType const graphType) -> void;

        auto loadFromFile(std::filesystem::path const& filePath) -> bool;

        auto dump() -> ShaderGraphData;

        auto getScene() -> core::ref_ptr<Scene>;

        auto getComponentRegistry() -> editor::ComponentRegistry&;

        auto getGraphType() const -> ShaderGraphType;

        auto compile() -> core::ref_ptr<ShaderGraphResult>;

      private:
        editor::ComponentRegistry componentRegistry;
        core::ref_ptr<Scene> sceneGraph;
        ShaderGraphType graphType;
    };
} // namespace ionengine::tools::editor

namespace ionengine::core
{
    template <>
    struct SerializableEnum<tools::editor::ShaderGraphType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(tools::editor::ShaderGraphType::Lit, "shader/lit");
            archive.field(tools::editor::ShaderGraphType::Unlit, "shader/unlit");
        }
    };
} // namespace ionengine::core