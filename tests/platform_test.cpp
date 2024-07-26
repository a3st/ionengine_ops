// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "platform/platform.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(App, Launch_Test)
{
    auto app = platform::App::create("TestProject");
    app->setInputEventCallback([](platform::InputEvent const& event) {
        if (event.deviceType == platform::InputDeviceType::Keyboard)
        {
            if (event.state == platform::InputState::Pressed)
            {
                std::cout << "Key pressed: " << static_cast<uint32_t>(event.keyCode) << std::endl;
            }
            else
            {
                std::cout << "Key released: " << static_cast<uint32_t>(event.keyCode) << std::endl;
            }
        }
    });
    app->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}