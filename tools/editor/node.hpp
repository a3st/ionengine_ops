// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine::tools::editor
{
    class Node : public core::ref_counted_object
    {
      public:
        struct NodeSocketInfo
        {
            std::string socketName;
            std::string socketType;
        };

        Node(uint64_t const nodeID, std::string_view const nodeName, uint32_t const posX, uint32_t const posY,
             std::vector<NodeSocketInfo> const& inputs, std::vector<NodeSocketInfo> const& outputs,
             bool const isExpand);

        auto getNodeID() const -> uint64_t;

        auto getNodeName() const -> std::string_view;

        auto getInputSockets() const -> std::span<NodeSocketInfo const> const;

        auto getOutputSockets() const -> std::span<NodeSocketInfo const> const;

        auto getPosition() const -> std::tuple<uint32_t, uint32_t>;

        auto isExpanded() const -> bool;

        virtual auto generateInitialShaderCode() -> std::string = 0;

        virtual auto generateResourceShaderCode() -> std::string = 0;

        virtual auto generateComputeShaderCode() -> std::string = 0;

      private:
        uint64_t nodeID;
        std::string nodeName;
        std::vector<NodeSocketInfo> inputs;
        std::vector<NodeSocketInfo> outputs;
        uint32_t posX;
        uint32_t posY;
        bool isExpand;
    };
} // namespace ionengine::tools::editor