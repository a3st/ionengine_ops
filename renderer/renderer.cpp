// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>

using namespace ionengine::renderer;

Renderer::Renderer(platform::Window& window) : 
    _context(window, 2),
    _frame_graph(_context) {

    build_frame_graph(window.client_size().width, window.client_size().height, 2);
        
    asset::Technique technique("../../data/techniques/geometry.json5");
    _shader_prog.emplace(_context, technique);
    asset::Technique technique_2("../../data/techniques/offscreen.json5");
    _shader_prog_2.emplace(_context, technique_2);

    vertex_declaration = { 
        backend::VertexInputDesc { "POSITION", 0, backend::Format::RGB32, 0, 0 },
        backend::VertexInputDesc { "COLOR", 0, backend::Format::RGB32, 0, 0 },
    };

    _sampler = _context.device().create_sampler(
        backend::Filter::MinMagMipLinear, 
        backend::AddressMode::Wrap, 
        backend::AddressMode::Wrap, 
        backend::AddressMode::Wrap,
        8,
        backend::CompareOp::Always
    );

    auto quad = frontend::GeometryBuffer::quad(_context);
    _offscreen_quad.emplace(std::move(quad));

    asset::Model model("");
    _geom_triangle.emplace(_context, model.surfaces()[0], frontend::BufferUsage::Static);
}

void Renderer::render(scene::Scene& scene) {

    // -> Scene Data
    /*
        for(auto& object : scene.cache_objects[CacheType::Mesh]) {
            // -> renderable_queue.emplace(object, material, data);
        }
    */

    _context.submit_or_skip_upload_buffers();

    auto frame_texture = _context.get_or_wait_previous_frame();

    _frame_graph.bind_attachment(*_swapchain_buffer, frame_texture);
    _frame_graph.execute();

    _context.swap_buffers();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count) {

    _gbuffer_color_buffer = &_frame_graph.add_attachment(
        frontend::AttachmentDesc {
            .name = "gbuffer_color",
            .format = backend::Format::RGBA8,
            .width = width,
            .height = height,
            .flags = backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource
        }
    );

    frontend::CreateColorInfo gbuffer_color_info {
        .attachment = _gbuffer_color_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.1f, 0.5f, 0.2f, 1.0f)
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "gbuffer",
            .width = width,
            .height = height,
            .color_infos = std::span<frontend::CreateColorInfo const>(&gbuffer_color_info, 1)
        },
        [&](frontend::RenderPassContext const& context) {
            
            backend::Handle<backend::Pipeline> pipeline_target;

            auto it = _pipelines.find(context.render_pass().index());
            if(it != _pipelines.end()) {
                
                pipeline_target = it->second;

            } else {

                pipeline_target = _context.device().create_pipeline(
                    _shader_prog.value().layout(),
                    vertex_declaration,
                    _shader_prog.value().shaders(),
                    backend::RasterizerDesc { backend::FillMode::Solid, backend::CullMode::Front },
                    backend::DepthStencilDesc { backend::CompareOp::Always, false },
                    backend::BlendDesc { false, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add },
                    context.render_pass(),
                    backend::InvalidHandle<backend::CachePipeline>()
                );

                _pipelines.insert({ context.render_pass().index(), pipeline_target });
            }

            _context.device().bind_pipeline(context.command_list(), pipeline_target);
            _geom_triangle.value().bind(context.command_list());
        }
    );

    _swapchain_buffer = &_frame_graph.add_attachment("swapchain_buffer", backend::MemoryState::Present, backend::MemoryState::Present);

    std::array<frontend::CreateColorInfo, 1> present_colors;

    present_colors[0] = frontend::CreateColorInfo {
        .attachment = _swapchain_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.0f, 0.3f, 0.8f, 1.0f)
    };

    std::array<frontend::Attachment*, 1> present_inputs;

    present_inputs[0] = {
        _gbuffer_color_buffer
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "present_only_pass",
            .width = width,
            .height = height,
            .color_infos = present_colors,
            .inputs = present_inputs
        },
        [&](frontend::RenderPassContext const& context) {

            backend::Handle<backend::Pipeline> pipeline_target;

            auto it = _pipelines.find(context.render_pass().index());
            if(it != _pipelines.end()) {
                
                pipeline_target = it->second;

            } else {

                pipeline_target = _context.device().create_pipeline(
                    _shader_prog_2.value().layout(),
                    _offscreen_quad.value().vertex_declaration(),
                    _shader_prog_2.value().shaders(),
                    backend::RasterizerDesc { backend::FillMode::Solid, backend::CullMode::None },
                    backend::DepthStencilDesc { backend::CompareOp::Always, false },
                    backend::BlendDesc { false, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add },
                    context.render_pass(),
                    backend::InvalidHandle<backend::CachePipeline>()
                );

                _pipelines.insert({ context.render_pass().index(), pipeline_target });
            }

            _context.device().bind_pipeline(context.command_list(), pipeline_target);

            frontend::ShaderUniformBinder binder(_context, _shader_prog_2.value());
            binder.bind_texture(_shader_prog_2.value().get_uniform_by_name("albedo"), context.attachment(0), _sampler);
            binder.update(context.command_list());

            _offscreen_quad.value().bind(context.command_list());
        }
    );

    _frame_graph.build(buffered_frame_count);
}

void Renderer::load_shader() {

}
