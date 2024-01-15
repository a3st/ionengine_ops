// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "render_graph.hpp"
#include "camera.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Backend;

class RenderPipeline : public core::ref_counted_object {
public:

    RenderPipeline() = default;

    virtual auto setup(
        RenderGraphBuilder& builder, 
        core::ref_ptr<Camera> camera, 
        uint32_t const width, 
        uint32_t const height
    ) -> std::vector<RGAttachment> = 0;
};

}

}