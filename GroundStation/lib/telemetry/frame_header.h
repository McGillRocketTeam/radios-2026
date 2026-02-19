#pragma once
#ifdef __cplusplus
#include <cstdint>

// ---- flags (1 = set) ----
#define FLAG_CTS 0x01
#define FLAG_ACK 0x02

// ---- 8-byte frame header ----
typedef struct  __attribute__((packed)) {
    uint16_t seq;            // 2B sequence number
    uint8_t  flags;          // bit0=CTS, bit1=ACK, bits2-7 reserved
    uint8_t  ack_id;         // 0 if unused
    uint32_t atomics_bitmap; // bit i => atomic i present in payload
} FrameHeader;

static_assert(sizeof(FrameHeader) == 8, "FrameHeader must be 8 bytes");

typedef union {
    FrameHeader data;
    uint8_t     bytes[sizeof(FrameHeader)];
} FrameHeaderPacket;

static inline bool header_has_cts(const FrameHeader* h) { return (h->flags & FLAG_CTS) != 0; }
static inline bool header_has_ack(const FrameHeader* h) { return (h->flags & FLAG_ACK) != 0; }
static inline uint8_t header_get_ack_id(const FrameHeader* h) { return h->ack_id; }
#endif
