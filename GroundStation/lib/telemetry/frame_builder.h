#pragma once
#include <cstdint>
#include <cstring>
#include "frame_helpers.h"
#include "frame_header.h"
#include "frame_view.h"

struct FrameBuilder {
    FrameHeader h{};
    uint8_t* out{nullptr};
    size_t   cap{0};
    size_t   used{sizeof(FrameHeader)};

    FrameBuilder(uint8_t* buf, size_t capacity) : out(buf), cap(capacity) {}

    bool addAtomic(int idx, const void* src, size_t sz) {
        if (AT_SIZE[idx] != sz) return false;
        if (used + sz > cap)    return false;
        std::memcpy(out + used, src, sz);
        used += sz;
        h.atomics_bitmap |= (1u << idx);
        return true;
    }

    size_t finalize(uint16_t seq, uint8_t flags, uint8_t ack_id) {
        h.seq    = seq;
        h.flags  = flags;
        h.ack_id = ack_id;
        std::memcpy(out, &h, sizeof(FrameHeader));
        return used;
    }
};
