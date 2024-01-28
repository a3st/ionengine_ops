// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "primitive_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

PrimitiveCache::PrimitiveCache(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) : device(&device), allocator(allocator) {

}

auto PrimitiveCache::get(PrimitiveData const& data, bool const immediate) -> std::optional<Primitive> {

    auto result = entries.find(data.hash);
    if(result != entries.end()) {
        if(result->second.vertices.get_result() && result->second.indices.get_result()) {
            return std::make_optional(
                std::make_pair(result->second.vertices.get(), result->second.indices.get())
            );
        } else {
            return std::nullopt;
        }
    } else {
        auto resource = TimedResource {
            .vertices = device->create_buffer(
                allocator, 
                data.vertices.size(), 
                (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex),
                data.vertices
            ),
            .indices = device->create_buffer(
                allocator,
                data.indices.size(),
                (rhi::BufferUsageFlags)(rhi::BufferUsage::Index),
                data.indices
            ),
            .lifetime = 0
        };

        std::optional<Primitive> ret = std::nullopt;
        if(immediate) {
            ret = std::make_optional(
                std::make_pair(result->second.vertices.get(), result->second.indices.get())
            );
        }
       
        entries.emplace(data.hash, std::move(resource));
        return ret;
    }
}

auto PrimitiveCache::update(float const dt) -> void {

    size = 0;
}