#pragma once

#include "telemetryInfo.h"
#include <U8g2lib.h>
#include <cstdint>
#include <cstddef>

void formatTelemetryLine(
    const TelemetryInfo& info,
    const TelemetryFieldDescriptor& desc,
    char* out,
    size_t outSize
);

void drawTelemetryPage(U8G2& display, const TelemetryInfo& info, uint8_t startIndex);