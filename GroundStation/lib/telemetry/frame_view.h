#pragma once
#include <cstddef>
#include <cstdint>
#include "frame_header.h"
#include "frame_helpers.h"

enum class ParseError : uint8_t { Ok=0, TooShort, PayloadTooShort, UnknownAtomicSize };

class FrameView {
public:
    FrameView();
    FrameView(const uint8_t* bytes, size_t len);
    void reset(const uint8_t* bytes, size_t len);

    ParseError validate() const;

    const FrameHeader* header() const;
    const uint8_t* payload() const;
    size_t payload_len() const;

    bool cts() const;
    bool ack() const;

    bool hasAtomic(int idx) const;
    const uint8_t* atomicPtr(int idx) const;

    template <typename T>
    const T* atomicAs(int idx) const {
        const uint8_t* p = atomicPtr(idx);
        return p ? reinterpret_cast<const T*>(p) : nullptr;
    }

private:
    const uint8_t* _base;
    size_t _len;
};
