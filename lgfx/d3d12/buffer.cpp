// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "buffer.h"
#include "device.h"

using namespace lgfx;

Buffer::Buffer(Device* device, MemoryPool* pool, const BufferDesc& desc) : 
    pool_(pool), desc_(desc), resource_desc_{} {

    resource_desc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc_.Width = desc.size;
    resource_desc_.Height = 1;
    resource_desc_.MipLevels = 1;
    resource_desc_.DepthOrArraySize = 1;
    resource_desc_.SampleDesc.Count = 1;
    resource_desc_.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    if(desc_.flags & BufferFlags::kConstantBuffer) {
        resource_desc_.Width = (desc.size + 255) & ~255;
    }

    switch(pool_->GetType()) {
        case MemoryType::kDefault: {
            initial_state_ = D3D12_RESOURCE_STATE_COMMON;
            break;
        }
        case MemoryType::kReadBack:
        case MemoryType::kUpload: {
            initial_state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
            break;
        }
    }

    D3D12_RESOURCE_ALLOCATION_INFO alloc_info = device->device_->GetResourceAllocationInfo(0, 1, &resource_desc_);
    alloc_info_ = pool_->Allocate(alloc_info.SizeInBytes);
    
    THROW_IF_FAILED(device->device_->CreatePlacedResource(alloc_info_.heap->heap_.Get(), alloc_info_.offset, &resource_desc_, initial_state_, nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource_.GetAddressOf())));
}

Buffer::~Buffer() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}

std::byte* Buffer::Map() {

    std::byte* data;
    D3D12_RANGE range{};
    THROW_IF_FAILED(resource_->Map(0, &range, reinterpret_cast<void**>(&data)));
    return data;
}

void Buffer::Unmap() {
    
    D3D12_RANGE range{};
    resource_->Unmap(0, &range);
}