#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(RHI, DeviceOffRender_Test)
{
    rhi::RHICreateInfo rhiCreateInfo{.window = nullptr};

    auto device = rhi::Device::create(rhiCreateInfo);
}

TEST(RHI, DeviceSwapchain_Test)
{
    auto platform = platform::App::create("TestProject");

    rhi::RHICreateInfo rhiCreateInfo{.window = platform->getWindowHandle(),
                                     .instance = platform->getInstanceHandle(),
                                     .windowWidth = 800,
                                     .windowHeight = 600};

    auto device = rhi::Device::create(rhiCreateInfo);

    auto graphicsContext = device->createGraphicsContext();

    platform->setIdleCallback([&]() {
        graphicsContext->reset();

        auto backBuffer = device->requestBackBuffer();

        std::vector<rhi::RenderPassColorInfo> colors{
            rhi::RenderPassColorInfo{
                .texture = backBuffer,
                .loadOp = rhi::RenderPassLoadOp::Clear,
                .storeOp = rhi::RenderPassStoreOp::Store,
                .clearColor = {0.5f, 0.6f, 0.7f, 1.0f}
            }
        };

        graphicsContext->setViewport(0, 0, 800, 600);
        graphicsContext->setScissor(0, 0, 800, 600);

        graphicsContext->barrier(backBuffer, rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
        graphicsContext->beginRenderPass(colors, std::nullopt);
        graphicsContext->endRenderPass();
        graphicsContext->barrier(backBuffer, rhi::ResourceState::RenderTarget, rhi::ResourceState::Present);

        auto result = graphicsContext->execute();
        
        device->presentBackBuffer();

        result.wait();
    });
    platform->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}