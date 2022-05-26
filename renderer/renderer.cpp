// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <scene/scene_visitor.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/camera_node.h>
#include <scene/point_light_node.h>
#include <lib/scope_profiler.h>

using namespace ionengine;
using namespace ionengine::renderer;

namespace ionengine::renderer {

class MeshVisitor : public scene::SceneVisitor {
public:

    MeshVisitor(RenderQueue& render_queue, std::vector<PointLightData>& point_lights, std::vector<EditorInstance>& editor_instances) : 
        _render_queue(&render_queue), _point_lights(&point_lights), _editor_instances(&editor_instances) { }

    void operator()(scene::MeshNode& other) {

        if(other.mesh().is_ok()) {
            for(size_t i = 0; i < other.mesh()->surfaces().size(); ++i) {
                
                auto instance = SurfaceInstance {
                    .model = other.transform_global()
                };

                _render_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->surfaces()[i].material_index));
            }
        }
    }

    void operator()(scene::PointLightNode& other) { 

        auto point_light = PointLightData {
            .position = other.position(),
            .attenuation = other.light_attenuation(),
            .range = other.light_range(),
            .color = lib::math::Vector3f(other.light_color().r, other.light_color().g, other.light_color().b)
        };

        _point_lights->emplace_back(std::move(point_light));

        _editor_instances->emplace_back(other.editor_icon(), lib::math::Matrixf::translate(other.position()));
    }

    // Default
    void operator()(scene::TransformNode& other) { }
    void operator()(scene::CameraNode& other) { }

private:

    RenderQueue* _render_queue;
    std::vector<PointLightData>* _point_lights;
    std::vector<EditorInstance>* _editor_instances;
};

}

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager, lib::ThreadPool& thread_pool, RendererAssets const& assets) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = 2 }),
    _asset_manager(&asset_manager),
    _upload_manager(thread_pool, _device),
    _frame_graph(_device),
    _shader_cache(_device),
    _geometry_cache(_device),
    _pipeline_cache(_device),
    _texture_cache(_device),
    _default_assets(assets) {

    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    asset_manager.mesh_pool().event_dispatcher().add(std::move(mesh_sender));
    _mesh_event_receiver.emplace(std::move(mesh_receiver));

    auto [technique_sender, technique_receiver] = lib::make_channel<asset::AssetEvent<asset::Technique>>();
    asset_manager.technique_pool().event_dispatcher().add(std::move(technique_sender));
    _technique_event_receiver.emplace(std::move(technique_receiver));

    auto [texture_sender, texture_receiver] = lib::make_channel<asset::AssetEvent<asset::Texture>>();
    asset_manager.texture_pool().event_dispatcher().add(std::move(texture_sender));
    _texture_event_receiver.emplace(std::move(texture_receiver));

    _width = window.client_width();
    _height = window.client_height();

    _material_buffer.resize(1024);

    for(uint32_t i = 0; i < 2; ++i) {
        _world_pools.emplace_back(_device, 64, BufferPoolUsage::Dynamic);
        _object_pools.emplace_back(_device, 32, 64, BufferPoolUsage::Dynamic);
        _point_light_pools.emplace_back(_device, 512, 2, BufferPoolUsage::Dynamic);
    }

    _default_assets.deffered.wait();
    _default_assets.billboard.wait();
    _default_assets.quad.wait();

    recreate_gbuffer(_width, _height);

    for(uint32_t i = 0; i < 2; ++i) {
        _depth_stencils.emplace_back(GPUTexture::render_target(_device, backend::Format::D32, _width, _height, backend::TextureFlags::DepthStencil).value());
    }
}

Renderer::~Renderer() {

    _frame_graph.reset();
}

void Renderer::update(float const delta_time) {

    SCOPE_PROFILE()

    // Mesh Event Update
    /*{
        asset::AssetEvent<asset::Mesh> mesh_event;
        while(_mesh_event_receiver.value().try_receive(mesh_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& event) {
                    for(size_t i = 0; i < event.asset->surfaces().size(); ++i) {
                        //_geometry_cache.get(_upload_manager, *event.asset, static_cast<uint32_t>(i));
                    }
                },
                // Default
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, mesh_event.data);
        }
    }

    // Technique Event Update
    {
        asset::AssetEvent<asset::Technique> technique_event;
        while(_technique_event_receiver.value().try_receive(technique_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Loaded>& event) {
                    //_shader_cache.get(*event.asset);
                },
                // Default
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, technique_event.data);
        }
    }

    // Texture Event Update
    {
        asset::AssetEvent<asset::Texture> texture_event;
        while(_texture_event_receiver.value().try_receive(texture_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Loaded>& event) {
                    //_texture_cache.get(_upload_manager, *event.asset);
                },
                // Default
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, texture_event.data);
        }
    }*/
}

void Renderer::render(scene::Scene& scene) {

    SCOPE_PROFILE()

    _upload_manager.update();

    uint32_t const frame_index = _frame_graph.wait();
    scene::CameraNode* camera = scene.graph().find_by_name<scene::CameraNode>("MainCamera");

    _deffered_queue.clear();
    _point_lights.clear();
    _editor_instances.clear();

    _world_pools.at(frame_index).reset();
    _object_pools.at(frame_index).reset();
    _point_light_pools.at(frame_index).reset();

    MeshVisitor mesh_visitor(_deffered_queue, _point_lights, _editor_instances);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), mesh_visitor);

    camera->calculate_matrices();

    build_frame_graph(_width, _height, frame_index, camera);
    
    _frame_graph.execute();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

    if(_width != width || _height != height) {

        _frame_graph.reset();

        _device.recreate_swapchain(width, height);
        _width = width;
        _height = height;

        recreate_gbuffer(width, height);

        _depth_stencils.clear();
        
        for(uint32_t i = 0; i < 2; ++i) {
            _depth_stencils.emplace_back(GPUTexture::render_target(_device, backend::Format::D32, _width, _height, backend::TextureFlags::DepthStencil).value());
        }
    }
}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const frame_index, scene::CameraNode* camera) {

    auto depth_stencil_info = CreateDepthStencilInfo {
        .attachment = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    auto gbuffer_infos = std::array<CreateColorInfo, 4> {
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).positions,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).albedo,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).normals,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).roughness_metalness_ao,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        }
    };

    // WorldData build buffer
    ResourcePtr<GPUBuffer> world_buffer;
    {
        auto world_data = WorldData {
            .view = camera->transform_view(),
            .projection = camera->transform_projection(),
            .camera_position = camera->position(),
            .point_light_count = static_cast<uint32_t>(_point_lights.size())
            //.direction_light_count = 0,
            //.spot_light_count = 0
        };

        world_buffer = _world_pools.at(frame_index).allocate();
        _upload_manager.upload_buffer_data(world_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&world_data), sizeof(WorldData)));
    }

    // PointLightData build buffer
    ResourcePtr<GPUBuffer> point_light_buffer;
    {
        point_light_buffer = _point_light_pools.at(frame_index).allocate();
        _upload_manager.upload_buffer_data(point_light_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(_point_lights.data()), _point_lights.size() * sizeof(PointLightData)));
    }

    _frame_graph.add_pass(
        "gbuffer", 
        width,
        height,
        gbuffer_infos,
        std::nullopt,
        depth_stencil_info,
        [=](RenderPassContext const& context) {
            
            for(auto const& batch : _deffered_queue) {

                _material_barriers.clear();

                auto result = std::find_if(
                    batch.material->passes().begin(), 
                    batch.material->passes().end(), 
                    [&](auto const& element) { 
                        return element.name == "gbuffer"; 
                    }
                );
                
                auto [pipeline, shader_program] = _pipeline_cache.get(_shader_cache, *result->technique, result->parameters, context.render_pass);
                
                _device.bind_pipeline(context.command_list, pipeline);
                
                ShaderUniformBinder binder(_device, *shader_program);

                apply_material_parameters(binder, *shader_program, *batch.material, frame_index, _material_barriers);

                if(!_material_barriers.empty()) {
                    _device.barrier(context.command_list, _material_barriers);
                }

                std::vector<ObjectData> object_datas;
                for(auto const& instance : batch.instances) {
                    object_datas.emplace_back(instance.model);
                }

                // ObjectData build buffer
                ResourcePtr<GPUBuffer> object_buffer;
                {
                    object_buffer = _object_pools.at(frame_index).allocate();
                    _upload_manager.upload_buffer_data(object_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_datas.data()), object_datas.size() * sizeof(ObjectData)));
                }

                uint32_t const world_location = shader_program->location_uniform_by_name("world");
                binder.bind_resource(world_location, world_buffer->buffer);

                uint32_t const object_location = shader_program->location_uniform_by_name("object");
                binder.bind_resource(object_location, object_buffer->buffer);

                binder.update(context.command_list);

                auto geometry_buffer = _geometry_cache.get(_upload_manager, *batch.mesh, batch.surface_index);
                if(geometry_buffer.is_ok()) {
                    geometry_buffer->bind(_device, context.command_list);
                    geometry_buffer->draw_indexed(_device, context.command_list, static_cast<uint32_t>(batch.instances.size()));
                }
            }
        }
    );

    CreateColorInfo swapchain_info = {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

    auto gbuffer_input_infos = std::array<CreateInputInfo, 4> {
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).positions },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).albedo },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).normals },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).roughness_metalness_ao }
    };

    _frame_graph.add_pass(
        "deffered",
        width,
        height,
        std::span<CreateColorInfo const>(&swapchain_info, 1),
        gbuffer_input_infos,
        std::nullopt,
        [=](RenderPassContext const& context) {

            auto parameters = asset::MaterialPassParameters {
                .fill_mode = asset::MaterialPassFillMode::Solid,
                .cull_mode = asset::MaterialPassCullMode::None,
                .depth_stencil = false
            };

            auto [pipeline, shader_program] = _pipeline_cache.get(_shader_cache, *_default_assets.deffered, parameters, context.render_pass);

            _device.bind_pipeline(context.command_list, pipeline);

            ShaderUniformBinder binder(_device, *shader_program);

            uint32_t const world_location = shader_program->location_uniform_by_name("world");
            binder.bind_resource(world_location, world_buffer->buffer);

            uint32_t const point_light_location = shader_program->location_uniform_by_name("point_light");
            binder.bind_resource(point_light_location, point_light_buffer->buffer);

            uint32_t const positions_location = shader_program->location_uniform_by_name("positions");
            binder.bind_resource(positions_location, _gbuffers.at(frame_index).positions->texture);
            binder.bind_resource(positions_location + 1, _gbuffers.at(frame_index).positions->sampler);

            uint32_t const albedo_location = shader_program->location_uniform_by_name("albedo");
            binder.bind_resource(albedo_location, _gbuffers.at(frame_index).albedo->texture);
            binder.bind_resource(albedo_location + 1, _gbuffers.at(frame_index).albedo->sampler);

            uint32_t const normals_location = shader_program->location_uniform_by_name("normal");
            binder.bind_resource(normals_location, _gbuffers.at(frame_index).normals->texture);
            binder.bind_resource(normals_location + 1, _gbuffers.at(frame_index).normals->sampler);

            uint32_t const roughmetal_location = shader_program->location_uniform_by_name("roughmetal");
            binder.bind_resource(roughmetal_location, _gbuffers.at(frame_index).roughness_metalness_ao->texture);
            binder.bind_resource(roughmetal_location + 1, _gbuffers.at(frame_index).roughness_metalness_ao->sampler);

            binder.update(context.command_list);

            auto geometry_buffer = _geometry_cache.get(_upload_manager, *_default_assets.quad, 0);
            if(geometry_buffer.is_ok()) {
                geometry_buffer->bind(_device, context.command_list);
                geometry_buffer->draw_indexed(_device, context.command_list, 1);
            }
        }
    );

    /*if(_editor_mode) {

        CreateColorInfo editor_info = {
            .attachment = nullptr,
            .load_op = backend::RenderPassLoadOp::Load,
            .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
        };

        _frame_graph.value().add_pass(
            "editor",
            width,
            height,
            std::span<CreateColorInfo const>(&editor_info, 1),
            gbuffer_input_infos,
            std::nullopt,
            [=](RenderPassContext const& context) {

                auto parameters = asset::MaterialPassParameters {
                    .fill_mode = asset::MaterialPassFillMode::Solid,
                    .cull_mode = asset::MaterialPassCullMode::None,
                    .depth_stencil = false
                };

                auto geometry = _geometry_cache.value().get(_upload_context.value(), *_quad_mesh, 0);
                auto [pipeline, shader_program] = _pipeline_cache.value().get(_shader_cache.value(), *_billboard_technique, parameters, context.render_pass());
            
                pipeline->bind(context.command_list());

                ShaderUniformBinder binder(_device, *shader_program);

                uint32_t const world_index = shader_program->location_by_uniform_name("world");
                binder.bind_buffer(world_index, *world_buffer);

                std::shared_ptr<GPUBuffer> objects_buffer = _object_sbuffer_pools.at(frame_index).allocate();

                std::vector<ObjectSBuffer> objects;

                std::shared_ptr<GPUTexture> gpu_texture;

                for(auto const& instance : _editor_instances) {

                    gpu_texture = _texture_cache.value().get(_upload_context.value(), *instance.icon);
                    
                    uint32_t const icon_index = shader_program->location_by_uniform_name("icon");
                    binder.bind_texture(icon_index, *gpu_texture);

                    objects.emplace_back(instance.model);
                }

                gpu_texture->barrier(context.command_list(), backend::MemoryState::ShaderRead);

                objects_buffer->copy_data(_upload_context.value(), std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(objects.data()), objects.size() * sizeof(ObjectSBuffer)));

                uint32_t const object_location = shader_program->location_by_uniform_name("object");
                binder.bind_buffer(object_location, *objects_buffer);

                binder.update(context.command_list());

                geometry->draw(context.command_list(), static_cast<uint32_t>(_editor_instances.size()));

                gpu_texture->barrier(context.command_list(), backend::MemoryState::Common);
            }
        );
    }*/
}

void Renderer::recreate_gbuffer(uint32_t const width, uint32_t const height) {

    _gbuffers.clear();

    for(uint32_t i = 0; i < 2; ++i) {

        auto gbuffer = GBufferData {
            .positions = GPUTexture::render_target(_device, backend::Format::RGBA16_FLOAT, _width, _height, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .albedo = GPUTexture::render_target(_device, backend::Format::RGBA8, _width, _height, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .normals = GPUTexture::render_target(_device, backend::Format::RGBA16_FLOAT, _width, _height, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .roughness_metalness_ao = GPUTexture::render_target(_device, backend::Format::RGBA8, _width, _height, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value()
        };

        _gbuffers.emplace_back(std::move(gbuffer));
    }
}

void Renderer::apply_material_parameters(
    ShaderUniformBinder& binder, 
    ShaderProgram& shader_program,
    asset::Material& material, 
    uint32_t const frame_index,
    std::vector<backend::MemoryBarrierDesc>& barriers
) {

    for(auto& [parameter_name, parameter] : material.parameters()) {

        if(parameter.is_sampler2D()) {

            auto it = shader_program.uniforms.find(parameter_name);
            if(it == shader_program.uniforms.end()) {
                break;
            }

            if(parameter.as_sampler2D().asset.is_ok()) {

                auto texture = _texture_cache.get(_upload_manager, *parameter.as_sampler2D().asset);

                if(texture.is_ok()) {
                
                    uint32_t const descriptor_location = it->second.as_sampler2D().index;
                    binder.bind_resource(descriptor_location, texture->texture);
                    binder.bind_resource(descriptor_location + 1, texture->sampler);

                    if(texture->memory_state != backend::MemoryState::ShaderRead) {
                        barriers.emplace_back(texture->texture, texture->memory_state, backend::MemoryState::ShaderRead);
                        texture->memory_state = backend::MemoryState::ShaderRead;
                    }
                }
            }

        } else {
                        
            auto it = shader_program.uniforms.find("material");
            if(it == shader_program.uniforms.end()) {
                break;
            }

            ShaderUniformData<ShaderUniformType::CBuffer> const& cbuffer_data = it->second.as_cbuffer();

            auto parameter_visitor = make_visitor(
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32> const& data) {
                    std::memcpy(_material_buffer.data() + cbuffer_data.offsets.at(parameter_name), &data.value, sizeof(float));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x2> const& data) {
                    std::memcpy(_material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector2f));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x3> const& data) {
                    std::memcpy(_material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector3f));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x4> const& data) {
                    std::memcpy(_material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector4f));
                },
                // Default
                [&](asset::MaterialParameterData<asset::MaterialParameterType::Sampler2D> const& parameter_data) { }
            );

            std::visit(parameter_visitor, parameter.data);
        }
    }

    // MaterialData build buffer
    /*ResourcePtr<GPUBuffer> material_buffer;
    {
        material_buffer = _material_pools.at(frame_index).allocate();
        _upload_manager.upload_buffer_data(material_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(_material_buffer.data()), _material_buffer.size()));
    }*/

    //uint32_t const material_location = binder.shader_program().location_by_uniform_name("material");
    //binder.bind_cbuffer(material_location, *buffer);
}

void Renderer::editor_mode(bool const enable) {
    _editor_mode = enable;
}