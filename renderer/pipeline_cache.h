// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_cache.h>
#include <renderer/pipeline.h>
#include <asset/material.h>

namespace ionengine::renderer {

class PipelineCache {
public:

    PipelineCache(backend::Device& device);

    PipelineCache(PipelineCache const&) = delete;

    PipelineCache(PipelineCache&& other) noexcept;

    PipelineCache& operator=(PipelineCache const&) = delete;

    PipelineCache& operator=(PipelineCache&& other) noexcept;

    std::shared_ptr<Pipeline> get(
        ShaderCache& shader_cache,
        asset::Material& material,
        std::string_view const render_pass_name, 
        backend::Handle<backend::RenderPass> const& render_pass
    );

private:

    backend::Device* _device;

    std::unordered_map<uint32_t, std::shared_ptr<Pipeline>> _data;
};

}