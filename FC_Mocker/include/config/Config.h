#pragma once
#include "frame_header.h"
#include "telemetry_packets.h"

// Frequency value in MHz
constexpr float FREQUENCY = 903.0f;

// Refer to LoRa Datasheet for allowed bandwidths, in kHz for RadioLib
constexpr float BANDWIDTH = 250.00f;

// Spreading factor (range from 7-12)
constexpr int SPREADING_FACTOR = 8;

// Output power, in dBm
constexpr int POWER_OUTPUT = 22;

size_t payload_size() {
    size_t size = 0;

    for (int i = 0; i < AT_TOTAL; ++i) {
        size += AT_SIZE[i];
    }

    return size;
}
const size_t MAX_FRAME_SIZE = sizeof(FrameHeader) + payload_size();