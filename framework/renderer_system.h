// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"

namespace ionengine {

class RenderSystem final : public Singleton<RenderSystem> {
DECLARE_SINGLETON(RenderSystem)
public:

protected:

    RenderSystem() {
        
    }

private:
};

}