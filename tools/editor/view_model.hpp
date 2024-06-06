// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine.hpp"
#include "registry.hpp"
#include "scene.hpp"
#include <webview.hpp>

namespace ionengine::tools::editor
{
    class ViewModel : public Engine
    {
      public:
        ViewModel(libwebview::App* app);

        auto requestPreviewImage() -> std::string;

        auto addContextItems() -> std::string;

        auto addResourceTypes() -> std::string;

        auto addShaderDomains() -> std::string;

        auto compileShader(std::string sceneData) -> std::string;

      protected:
        auto init() -> void override;

        auto update(float const dt) -> void override;

        auto render() -> void override;

      private:
        libwebview::App* app;
        editor::ComponentRegistry componentRegistry;

        core::ref_ptr<TextureAsset> previewImage;
        core::ref_ptr<Scene> shaderGraph;

        auto compileShaderGraph() -> bool;
    };
} // namespace ionengine::tools::editor