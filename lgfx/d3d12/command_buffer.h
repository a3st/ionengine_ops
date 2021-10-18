// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class CommandBuffer {

friend class Device;

public:

    CommandBuffer();
    ~CommandBuffer();
    CommandBuffer(Device* device, const CommandBufferType type);
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&& rhs) noexcept;

    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&& rhs) noexcept;

    void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void SetScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);
    void BeginRenderPass(RenderPass* render_pass, FrameBuffer* frame_buffer, const ClearValueDesc& desc);
    void EndRenderPass();
    void TextureMemoryBarrier(Texture* texture, const MemoryState before, const MemoryState after);
    void Reset();
    void Close();
    
private:

    Device* device_;

    ID3D12CommandAllocator* allocator_;
    ID3D12GraphicsCommandList4* list_;
};

}