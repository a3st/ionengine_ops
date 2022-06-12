// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/shader.h>
#include <asset/material.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class ShaderCache {
public:

    ShaderCache(backend::Device& device);

    ShaderCache(ShaderCache const&) = delete;

    ShaderCache(ShaderCache&& other) noexcept;

    ShaderCache& operator=(ShaderCache const&) = delete;

    ShaderCache& operator=(ShaderCache&& other) noexcept;

    ResourcePtr<Shader> get(asset::AssetPtr<asset::Material> material, std::string_view const pass_shader_name);

private:

    backend::Device* _device;

    std::unordered_map<std::string, CacheEntry<ResourcePtr<Shader>>> _data;
};

}
