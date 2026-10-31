#pragma once

#include "telemetryInfo.h"

struct TelemetryPageDescriptor
{
    const char* name;
    const TelemetryFieldDescriptor* const* fields;
    size_t fieldCount;
};

// References to the shared parameter descriptors: FC/GS RSSI and SNR, packet ID.
static const TelemetryFieldDescriptor* const SIGNAL_QUALITY_FIELDS[] = {
    &TELEMETRY_FIELDS[6], &TELEMETRY_FIELDS[7],
    &TELEMETRY_FIELDS[8], &TELEMETRY_FIELDS[9], &TELEMETRY_FIELDS[5]
};

// GPS latitude, longitude, altitude, age, and barometric altitude.
static const TelemetryFieldDescriptor* const FLIGHT_COMPUTER_FIELDS[] = {
    &TELEMETRY_FIELDS[0], &TELEMETRY_FIELDS[1], &TELEMETRY_FIELDS[2],
    &TELEMETRY_FIELDS[3], &TELEMETRY_FIELDS[4]
};

static const TelemetryPageDescriptor TELEMETRY_PAGES[] = {
    { "Signal quality", SIGNAL_QUALITY_FIELDS,
      sizeof(SIGNAL_QUALITY_FIELDS) / sizeof(SIGNAL_QUALITY_FIELDS[0]) },
    { "Flight computer", FLIGHT_COMPUTER_FIELDS,
      sizeof(FLIGHT_COMPUTER_FIELDS) / sizeof(FLIGHT_COMPUTER_FIELDS[0]) }
};

static constexpr size_t TELEMETRY_PAGE_COUNT =
    sizeof(TELEMETRY_PAGES) / sizeof(TELEMETRY_PAGES[0]);
