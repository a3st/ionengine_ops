// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "node.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Node::Node(uint64_t const nodeID, std::string_view const nodeName, uint32_t const posX, uint32_t const posY,
               std::vector<NodeSocketInfo> const& inputs, std::vector<NodeSocketInfo> const& outputs)
        : nodeID(nodeID), nodeName(nodeName), posX(posX), posY(posY), inputs(inputs), outputs(outputs)
    {
    }

    auto Node::getNodeID() const -> uint64_t
    {
        return nodeID;
    }

    auto Node::getNodeName() const -> std::string_view
    {
        return nodeName;
    }

    auto Node::getInputSockets() const -> std::span<NodeSocketInfo const> const
    {
        return inputs;
    }

    auto Node::getOutputSockets() const -> std::span<NodeSocketInfo const> const
    {
        return outputs;
    }

    auto Node::getPosition() const -> std::tuple<uint32_t, uint32_t>
    {
        return {posX, posY};
    }
} // namespace ionengine::tools::editor