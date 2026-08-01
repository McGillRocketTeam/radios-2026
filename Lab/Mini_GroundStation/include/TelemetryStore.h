#pragma once

#include <cstddef>
#include <cstdint>

#include "telemetryInfo.h"

class TelemetryStore
{
public:
    // Updates the latest snapshot from one valid ASTRA telemetry frame.
    // Atomics omitted from a frame retain their last received values.
    bool updateFromFrame(const uint8_t* frame, size_t length);

    const TelemetryInfo& latest() const;
    uint32_t revision() const;
    bool hasTelemetry() const;

private:
    TelemetryInfo latest_{};
    uint32_t revision_ = 0;
    bool hasTelemetry_ = false;
};
