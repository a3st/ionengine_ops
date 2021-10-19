// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class TextureView {

public:

    TextureView();
    ~TextureView();
    TextureView(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc);
    TextureView(const TextureView&) = delete;
    TextureView(TextureView&& rhs) noexcept;

    TextureView& operator=(const TextureView&) = delete;
    TextureView& operator=(TextureView&& rhs) noexcept;

    inline Texture* GetTexture() const { return texture_; }
    inline const TextureViewDesc& GetViewDesc() const { return view_desc_; }
    inline DescriptorPtr GetDescriptorPtr() const { return ptr_; }

private:

    DescriptorPool* pool_;
    Texture* texture_;
    DescriptorPtr ptr_;
    TextureViewDesc view_desc_;

    DescriptorPtr CreateRenderTarget(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc);
};

}