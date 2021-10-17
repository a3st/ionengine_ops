// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

DXGI_FORMAT gfx_to_dxgi_format(const Format format) {

	switch(format) {
		case Format::Unknown: return DXGI_FORMAT_UNKNOWN;
		case Format::RGBA32float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::RGBA32uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::RGBA32int: return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::RGB32float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::RGB32uint: return DXGI_FORMAT_R32G32B32_UINT;
		case Format::RGB32int: return DXGI_FORMAT_R32G32B32_SINT;
		case Format::RG32float: return DXGI_FORMAT_R32G32_FLOAT;
		case Format::RG32uint: return DXGI_FORMAT_R32G32_UINT;
		case Format::RG32int: return DXGI_FORMAT_R32G32_SINT;
		case Format::R32float: return DXGI_FORMAT_R32_FLOAT;
		case Format::R32uint: return DXGI_FORMAT_R32_UINT;
		case Format::R32int: return DXGI_FORMAT_R32_SINT;
		case Format::RGBA16float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case Format::RGBA16uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::RGBA16int: return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::RGBA16unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::RGBA16snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::RG16float: return DXGI_FORMAT_R16G16_FLOAT;
		case Format::RG16uint: return DXGI_FORMAT_R16G16_UINT;
		case Format::RG16int: return DXGI_FORMAT_R16G16_SINT;
		case Format::RG16unorm: return DXGI_FORMAT_R16G16_UNORM;
		case Format::RG16snorm: return DXGI_FORMAT_R16G16_SNORM;
		case Format::R16float: return DXGI_FORMAT_R16_FLOAT;
		case Format::R16uint: return DXGI_FORMAT_R16_UINT;
		case Format::R16int: return DXGI_FORMAT_R16_SINT;
		case Format::R16unorm: return DXGI_FORMAT_R16_UNORM;
		case Format::R16snorm: return DXGI_FORMAT_R16_SNORM;
		case Format::RGBA8uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::RGBA8int: return DXGI_FORMAT_R8G8B8A8_SINT;
		case Format::RGBA8unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA8snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::RG8uint: return DXGI_FORMAT_R8G8_UINT;
		case Format::RG8int: return DXGI_FORMAT_R8G8_SINT;
		case Format::RG8unorm: return DXGI_FORMAT_R8G8_UNORM;
		case Format::RG8snorm: return DXGI_FORMAT_R8G8_SNORM;
		case Format::R8uint: return DXGI_FORMAT_R8_UINT;
		case Format::R8int: return DXGI_FORMAT_R8_SINT;
		case Format::R8unorm: return DXGI_FORMAT_R8_UNORM;
		case Format::R8snorm: return DXGI_FORMAT_R8_SNORM;
		default: assert(false && "passed invalid argument to gfx_to_dxgi_format"); return DXGI_FORMAT_UNKNOWN;
	}
}

Format dxgi_format_to_gfx(const DXGI_FORMAT format) {

	switch(format) {
		case DXGI_FORMAT_UNKNOWN: return Format::Unknown;
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return Format::RGBA32float;
		case DXGI_FORMAT_R32G32B32A32_UINT: return Format::RGBA32uint;
		case DXGI_FORMAT_R32G32B32A32_SINT: return Format::RGBA32int;
		case DXGI_FORMAT_R32G32B32_FLOAT: return Format::RGB32float;
		case DXGI_FORMAT_R32G32B32_UINT: return Format::RGB32uint;
		case DXGI_FORMAT_R32G32B32_SINT: return Format::RGB32int;
		case DXGI_FORMAT_R32G32_FLOAT: return Format::RG32float;
		case DXGI_FORMAT_R32G32_UINT: return Format::RG32uint;
		case DXGI_FORMAT_R32G32_SINT: return Format::RG32int;
		case DXGI_FORMAT_R32_FLOAT: return Format::R32float;
		case DXGI_FORMAT_R32_UINT: return Format::R32uint;
		case DXGI_FORMAT_R32_SINT: return Format::R32int;
		case DXGI_FORMAT_R16G16B16A16_FLOAT: return Format::RGBA16float;
		case DXGI_FORMAT_R16G16B16A16_UINT: return Format::RGBA16uint;
		case DXGI_FORMAT_R16G16B16A16_SINT: return Format::RGBA16int;
		case DXGI_FORMAT_R16G16B16A16_UNORM: return Format::RGBA16unorm;
		case DXGI_FORMAT_R16G16B16A16_SNORM: return Format::RGBA16snorm;
		case DXGI_FORMAT_R16G16_FLOAT: return Format::RG16float;
		case DXGI_FORMAT_R16G16_UINT: return Format::RG16uint;
		case DXGI_FORMAT_R16G16_SINT: return Format::RG16int;
		case DXGI_FORMAT_R16G16_UNORM: return Format::RG16unorm;
		case DXGI_FORMAT_R16G16_SNORM: return Format::RG16snorm;
		case DXGI_FORMAT_R16_FLOAT: return Format::R16float;
		case DXGI_FORMAT_R16_UINT: return Format::R16uint;
		case DXGI_FORMAT_R16_SINT: return Format::R16int;
		case DXGI_FORMAT_R16_UNORM: return Format::R16unorm;
		case DXGI_FORMAT_R16_SNORM: return Format::R16snorm;
		case DXGI_FORMAT_R8G8B8A8_UINT: return Format::RGBA8uint;
		case DXGI_FORMAT_R8G8B8A8_SINT: return Format::RGBA8int;
		case DXGI_FORMAT_R8G8B8A8_UNORM: return Format::RGBA8unorm;
		case DXGI_FORMAT_R8G8B8A8_SNORM: return Format::RGBA8snorm;
		case DXGI_FORMAT_R8G8_UINT: return Format::RG8uint;
		case DXGI_FORMAT_R8G8_SINT: return Format::RG8int;
		case DXGI_FORMAT_R8G8_UNORM: return Format::RG8unorm;
		case DXGI_FORMAT_R8G8_SNORM: return Format::RG8snorm;
		case DXGI_FORMAT_R8_UINT: return Format::R8uint;
		case DXGI_FORMAT_R8_SINT: return Format::R8int;
		case DXGI_FORMAT_R8_UNORM: return Format::R8unorm;
		case DXGI_FORMAT_R8_SNORM: return Format::R8snorm;
		default: assert(false && "passed invalid argument to gfx_to_dxgi_format"); return Format::Unknown;
	}
}

D3D12_FILL_MODE gfx_to_d3d12_fill_mode(const FillMode fill_mode) {
	
	switch(fill_mode) {
		case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_fill_mode"); return D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE gfx_to_d3d12_cull_mode(const CullMode cull_mode) {
	
	switch(cull_mode) {
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_cull_mode"); return D3D12_CULL_MODE_NONE;
	}
}

D3D12_COMPARISON_FUNC gfx_to_d3d12_comparison_func(const ComparisonFunc comparison_func) {
	
	switch(comparison_func) {
		case ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		case ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_comparison_func"); return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

D3D12_STENCIL_OP gfx_to_d3d12_stencil_op(const StencilOp stencil_op) {
	
	switch(stencil_op) {
		case StencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case StencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case StencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_stencil_op"); return D3D12_STENCIL_OP_ZERO;
	}
}

D3D12_BLEND gfx_to_d3d12_blend(const Blend blend) {
	
	switch(blend) {
		case Blend::Zero: return D3D12_BLEND_ZERO;
		case Blend::One: return D3D12_BLEND_ONE;
		case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_blend"); return D3D12_BLEND_ZERO;
	}
}

D3D12_BLEND_OP gfx_to_d3d12_blend_op(const BlendOp blend_op) {
	
	switch(blend_op) {
		case BlendOp::Add: return D3D12_BLEND_OP_ADD;
		case BlendOp::Min: return D3D12_BLEND_OP_MIN;
		case BlendOp::Max: return D3D12_BLEND_OP_MAX;
		case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_blend_op"); return static_cast<D3D12_BLEND_OP>(0);
	}
}

D3D12_SHADER_VISIBILITY gfx_to_d3d12_shader_visibility(const ShaderType shader_type) {
	
	switch(shader_type) {
		case ShaderType::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderType::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderType::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderType::Hull: return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderType::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderType::All: return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderType::Compute: return D3D12_SHADER_VISIBILITY_ALL;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_shader_visibility"); return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_DESCRIPTOR_RANGE_TYPE gfx_to_d3d12_descriptor_range_type(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_descriptor_range_type"); return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}
}

D3D12_COMMAND_LIST_TYPE gfx_to_d3d12_command_list_type(const CommandListType command_list_type) {
	
	switch(command_list_type) {
        case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_command_list_type"); return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

D3D12_HEAP_TYPE gfx_to_d3d12_heap_type(const MemoryType memory_type) {
	
	switch(memory_type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_heap_type"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_RESOURCE_STATES gfx_to_d3d12_resource_state(const ResourceState resource_state) {
	
	switch(resource_state) {
		case ResourceState::Common: return D3D12_RESOURCE_STATE_COMMON;
		case ResourceState::VertexAndConstantBuffer: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case ResourceState::IndexBuffer: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case ResourceState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case ResourceState::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case ResourceState::DepthStencilWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case ResourceState::DepthStencilRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
		case ResourceState::NonPixelShaderResource: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case ResourceState::PixelShaderResource: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case ResourceState::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
		case ResourceState::CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case ResourceState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		case ResourceState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_resource_state"); return D3D12_RESOURCE_STATE_COMMON;
	}
}

D3D12_RESOURCE_DIMENSION gfx_to_d3d12_resource_dimension(const ViewDimension view_dimension) {
	
	switch(view_dimension) {
		case ViewDimension::Buffer: return D3D12_RESOURCE_DIMENSION_BUFFER;
		case ViewDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ViewDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ViewDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_resource_dimension"); return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_FILTER gfx_to_d3d12_filter(const Filter filter) {
	
	switch(filter) {
		case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
		case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_filter"); return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE gfx_to_d3d12_texture_address_mode(const TextureAddressMode address_mode) {
	
	switch(address_mode) {
		case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_texture_address_mode"); return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE gfx_to_d3d12_descriptor_heap_type(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: 
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer:
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case ViewType::RenderTarget: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case ViewType::DepthStencil: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_descriptor_heap_type"); return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE gfx_to_d3d12_render_pass_begin_type(const RenderPassLoadOp load_op) {
	
	switch (load_op) {
    	case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_render_pass_begin_type"); return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE gfx_to_d3d12_render_pass_end_type(const RenderPassStoreOp store_op) {
    
	switch (store_op) {
    	case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_render_pass_end_type"); return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

}
