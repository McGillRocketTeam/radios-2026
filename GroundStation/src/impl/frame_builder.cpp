#include <cstring>
#include "frame_builder.h"
#include "frame_header.h"
#include "telemetry_packets.h"

FrameBuilder::FrameBuilder(uint8_t* buf, size_t capacity)
    : out(buf), cap(capacity), used(sizeof(FrameHeader)) {}

bool FrameBuilder::addAtomic(int idx, const void* src, size_t sz) {
    if (idx < 0 || idx >= AT_TOTAL) return false;
    if (AT_SIZE[idx] != sz) return false;
    if (used + sz > cap)    return false;
    std::memcpy(out + used, src, sz);
    used += sz;
    h.atomics_bitmap |= (1u << idx);
    return true;
}

size_t FrameBuilder::finalize(uint16_t seq, uint8_t flags, uint8_t ack_id) {
    h.seq    = seq;
    h.flags  = flags;
    h.ack_id = ack_id;
    std::memcpy(out, &h, sizeof(FrameHeader));
    return used;
}
