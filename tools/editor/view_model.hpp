// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/engine.hpp"
#include "graph/registry.hpp"
#include "graph/scene.hpp"
#include <webview.hpp>
#include "asset_tree.hpp"
#include "shader_graph/shader_graph.hpp"

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

        auto getAssetTree() -> std::string;

        auto assetBrowserCreateFile(std::string fileData) -> std::string;

      protected:
        auto init() -> void override;

        auto update(float const dt) -> void override;

        auto render() -> void override;

      private:
        libwebview::App* app;

        ShaderGraphEditor shaderGraphEditor;

        editor::ComponentRegistry componentRegistry;

        core::ref_ptr<TextureAsset> previewImage;
        core::ref_ptr<Scene> shaderGraph;
        core::ref_ptr<ShaderAsset> previewShader;

        auto compileShaderGraph() -> bool;

        AssetTree assetTree;
    };
} // namespace ionengine::tools::editor