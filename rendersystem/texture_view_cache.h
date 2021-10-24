// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureViewCache {

    using Key = std::pair<lgfx::Texture*, lgfx::TextureViewDesc>;

public:

    TextureViewCache();
    TextureViewCache(lgfx::Device* device);
    TextureViewCache(const TextureViewCache&) = delete;
    TextureViewCache(TextureViewCache&& rhs) noexcept;

    TextureViewCache& operator=(const TextureViewCache&) = delete;
    TextureViewCache& operator=(TextureViewCache&& rhs) noexcept;

    lgfx::TextureView* GetTextureView(const Key& key);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<Key, lgfx::TextureView> texture_views_;

    lgfx::DescriptorPool rt_descriptor_pool_;
    lgfx::DescriptorPool ds_descriptor_pool_;
    lgfx::DescriptorPool sr_descriptor_pool_;
};
    
}
