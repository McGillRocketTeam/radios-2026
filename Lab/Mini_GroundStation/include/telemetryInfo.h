#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdio>

struct TelemetryInfo
{
    float gps_latitude_deg;
    float gps_longitude_deg;
    float gps_altitude_m;
    float gps_time_last_update_s;
    float baro_altitude_ft;
    uint16_t packet_sequence_number;
    uint8_t rssi_dbm;
    int8_t snr_db;
};

enum class FieldType : uint8_t
{
    FLOAT,
    UINT16,
    UINT8,
    INT8
};

struct TelemetryFieldDescriptor
{
    const char* label;
    FieldType type;
    size_t offset;
    uint8_t decimals;
    const char* unit;
};


static const TelemetryFieldDescriptor TELEMETRY_FIELDS[] =
{
    { "Lat",      FieldType::FLOAT, offsetof(TelemetryInfo, gps_latitude_deg),          6, " deg" },
    { "Lon",      FieldType::FLOAT, offsetof(TelemetryInfo, gps_longitude_deg),         6, " deg" },
    { "GPS Alt",  FieldType::FLOAT, offsetof(TelemetryInfo, gps_altitude_m),            2, " m"   },
    { "GPS Age",  FieldType::FLOAT, offsetof(TelemetryInfo, gps_time_last_update_s),    1, " s"   },
    { "Baro Alt", FieldType::FLOAT, offsetof(TelemetryInfo, baro_altitude_ft),          1, " ft"  },
    { "Packet",   FieldType::UINT16, offsetof(TelemetryInfo, packet_sequence_number),   0, ""     },
    { "FC RSSI",  FieldType::UINT8,  offsetof(TelemetryInfo, rssi_dbm),                 0, " dBm" },
    { "FC SNR",   FieldType::INT8,   offsetof(TelemetryInfo, snr_db),                   0, " dB"  }
};

static constexpr size_t TELEMETRY_FIELD_COUNT =
    sizeof(TELEMETRY_FIELDS) / sizeof(TELEMETRY_FIELDS[0]);

static constexpr size_t TELEMETRY_VISIBLE_FIELD_COUNT = 5;
