#pragma once
#include <cstddef>
#include <cstdint>
#include "frame_header.h"

struct FrameBuilder {
    FrameHeader h{0};
    uint8_t* out{nullptr};
    size_t cap{0};
    size_t used{0};

    uint8_t* temp_buffer;
    size_t temp_used{0};
    
    const void* atomic_pointers[32];

    FrameBuilder(uint8_t* buf, size_t capacity);

    bool addAtomic(int idx, const void* src, size_t sz);
    size_t finalize(uint16_t seq, uint8_t flags, uint8_t ack_id);
};