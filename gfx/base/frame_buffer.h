// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<class B = backend::base>
class FrameBuffer {
public:

    const FrameBufferDesc<B>& get_desc() const;
};

}