#pragma once
#include <cstddef>
#include <cstdint>
#include "frame_header.h"
#include "frame_helpers.h"

enum class ParseError : uint8_t { Ok=0, TooShort, PayloadTooShort, UnknownAtomicSize };

class FrameView {
public:
    FrameView(const uint8_t* bytes, size_t len) : base_(bytes), len_(len) {}

    ParseError validate() const {
        if (len_ < sizeof(FrameHeader)) return ParseError::TooShort;
        const auto* h = header();
        const uint32_t need = payload_length_from_bitmap(h->atomics_bitmap);
        if (sizeof(FrameHeader) + need > len_) return ParseError::PayloadTooShort;
        for (int i = 0; i < AT_TOTAL; ++i)
            if ((h->atomics_bitmap & (1u << i)) && AT_SIZE[i] == 0)
                return ParseError::UnknownAtomicSize;
        return ParseError::Ok;
    }

    const FrameHeader* header() const {
        return reinterpret_cast<const FrameHeader*>(base_);
    }
    const uint8_t* payload() const { return base_ + sizeof(FrameHeader); }
    size_t payload_len() const { return len_ - sizeof(FrameHeader); }

    bool hasAtomic(int idx) const { return (header()->atomics_bitmap & (1u << idx)) != 0; }

    const uint8_t* atomicPtr(int idx) const {
        if (!hasAtomic(idx)) return nullptr;
        const uint32_t off = atomic_offset(header()->atomics_bitmap, idx);
        if (off + AT_SIZE[idx] > payload_len()) return nullptr;
        return payload() + off;
    }

    template <typename T>
    const T* atomicAs(int idx) const {
        const uint8_t* p = atomicPtr(idx);
        return p ? reinterpret_cast<const T*>(p) : nullptr;
    }

private:
    const uint8_t* base_;
    size_t         len_;
};
