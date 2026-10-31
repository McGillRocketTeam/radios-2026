#pragma once

#include "telemetryPages.h"
#include <U8g2lib.h>
#include <cstdint>
#include <cstddef>

void formatTelemetryLine(
    const TelemetryInfo& info,
    const TelemetryFieldDescriptor& desc,
    char* out,
    size_t outSize
);

void drawTelemetryPage(U8G2& display, const TelemetryInfo& info,
                       const TelemetryPageDescriptor& page, uint8_t startIndex);
void drawTelemetryPageName(U8G2& display, const char* name);