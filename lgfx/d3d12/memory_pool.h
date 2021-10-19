// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const size_t kMemoryPoolDefaultHeapSize = 268435456;
const size_t kMemoryPoolDefaultBlockSize = 1048576;

struct MemoryHeap {

    ComPtr<ID3D12Heap> heap;

    size_t heap_size;

    uint64_t block_count;
    std::vector<uint8_t> blocks;

    uint64_t offset;

    MemoryHeap();
    MemoryHeap(Device* device, const uint64_t align, const MemoryType type, const MemoryFlags flags);
    MemoryHeap(const MemoryHeap&) = delete;
    MemoryHeap(MemoryHeap&& rhs) noexcept;

    MemoryHeap& operator=(const MemoryHeap&) = delete;
    MemoryHeap& operator=(MemoryHeap&& rhs) noexcept;
};

struct MemoryPtr {
    MemoryHeap* heap;
    uint64_t offset;
};

class MemoryPool {

public:

    MemoryPool();
    MemoryPool(Device* device, const size_t size, const uint64_t align, const MemoryType type, const MemoryFlags flags);
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&& rhs) noexcept;

    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&& rhs) noexcept;

    MemoryPtr Allocate(const size_t size);
    void Deallocate(MemoryPtr* ptr, const size_t size);

    inline MemoryType GetType() const { return type_; }
    inline MemoryFlags GetFlags() const { return flags_; }

private:

    MemoryType type_;
    MemoryFlags flags_;

    std::vector<MemoryHeap> heaps_;

    size_t AlignedBlockSize(const size_t size) const;
};

}