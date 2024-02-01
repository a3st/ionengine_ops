// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "command_buffer.hpp"
#include "core/ref_ptr.hpp"
#include "future.hpp"
#include "memory_allocator.hpp"
#include "texture.hpp"

namespace ionengine::platform
{
    class Window;
}

namespace ionengine::renderer::rhi
{
    enum class BackendType
    {
        DirectX12,
        Vulkan
    };

    inline uint32_t constexpr FRAMES_IN_FLIGHT = 2;
    inline uint32_t constexpr STAGING_BUFFER_SIZE = 32 * 1024 * 1024;

    class Device : public core::ref_counted_object
    {
      public:
        virtual ~Device() = default;

        static auto create(BackendType const backend_type, platform::Window const& window) -> core::ref_ptr<Device>;

        virtual auto create_allocator(size_t const block_size, size_t const chunk_size,
                                      MemoryAllocatorUsage const usage) -> core::ref_ptr<MemoryAllocator> = 0;

        virtual auto create_shader(std::span<uint8_t const> const data_bytes) -> core::ref_ptr<Shader> = 0;

        virtual auto create_texture(core::ref_ptr<MemoryAllocator> allocator, uint32_t const width,
                                    uint32_t const height, uint32_t const depth, uint32_t const mip_levels,
                                    TextureFormat const format, TextureDimension const dimension,
                                    TextureUsageFlags const flags, std::vector<std::span<uint8_t const>> const& data)
            -> Future<Texture> = 0;

        virtual auto allocate_command_buffer(CommandBufferType const buffer_type) -> core::ref_ptr<CommandBuffer> = 0;

        virtual auto create_buffer(core::ref_ptr<MemoryAllocator> allocator, size_t const size,
                                   BufferUsageFlags const flags, std::span<uint8_t const> const data)
            -> Future<Buffer> = 0;

        virtual auto write_buffer(core::ref_ptr<Buffer> buffer, std::span<uint8_t const> const data)
            -> Future<Buffer> = 0;

        virtual auto submit_command_lists(std::span<core::ref_ptr<CommandBuffer>> const command_buffers) -> void = 0;

        virtual auto wait_for_idle() -> void = 0;

        virtual auto request_next_swapchain_buffer() -> core::ref_ptr<Texture> = 0;

        virtual auto present() -> void = 0;

        virtual auto resize_swapchain_buffers(uint32_t const width, uint32_t const height) -> void = 0;
    };
} // namespace ionengine::renderer::rhi