#include "TelemetryStore.h"

#include <cstring>

#include "frame_header.h"
#include "telemetry_packets.h"

namespace
{
static_assert(AT_TOTAL < 32, "ASTRA atomic bitmap must fit in uint32_t");

size_t atomicSize(int atomicType)
{
    switch (atomicType)
    {
        case AT_RECOV_ATOMIC: return sizeof(recov_atomic_data);
        case AT_PROP_STATES_ATOMIC: return sizeof(prop_states_atomic_data);
        case AT_PROP_ATOMIC: return sizeof(prop_atomic_data);
        case AT_FLIGHT_STAGE_ATOMIC: return sizeof(flight_stage_atomic_data);
        case AT_FC_INTERNAL_ATOMIC: return sizeof(fc_internal_atomic_data);
        case AT_ALTITUDE_ATOMIC: return sizeof(altitude_atomic_data);
        case AT_ALTITUDE_EVENTS_ATOMIC: return sizeof(altitude_events_atomic_data);
        case AT_ACCELERATION_ATOMIC: return sizeof(acceleration_atomic_data);
        case AT_GYRO_ATOMIC: return sizeof(gyro_atomic_data);
        case AT_GPS_ATOMIC: return sizeof(gps_atomic_data);
        case AT_RADIO_ATOMIC: return sizeof(radio_atomic_data);
        case AT_SD_ATOMIC: return sizeof(sd_atomic_data);
        case AT_PAYLOAD_STATUS_ATOMIC: return sizeof(payload_status_atomic_data);
        case AT_PAYLOAD_DATA_ATOMIC: return sizeof(payload_data_atomic_data);
        case AT_PAYLOAD_ADAPTER0_ATOMIC: return sizeof(payload_adapter0_atomic_data);
        case AT_PAYLOAD_ADAPTER1_ATOMIC: return sizeof(payload_adapter1_atomic_data);
        case AT_PAYLOAD_ADAPTER_2_ATOMIC: return sizeof(payload_adapter_2_atomic_data);
        case AT_PAYLOAD_ADAPTER_3_ATOMIC: return sizeof(payload_adapter_3_atomic_data);
        case AT_GPS_DEBUG_ATOMIC: return sizeof(gps_debug_atomic_data);
        default: return 0;
    }
}

bool validateFrame(const FrameHeader& header, size_t length)
{
    const uint32_t knownAtomicsMask = (1UL << AT_TOTAL) - 1UL;
    if ((header.atomics_bitmap & ~knownAtomicsMask) != 0)
    {
        return false;
    }

    size_t requiredLength = sizeof(FrameHeader);
    for (int atomicType = 0; atomicType < AT_TOTAL; ++atomicType)
    {
        if ((header.atomics_bitmap & (1UL << atomicType)) == 0)
        {
            continue;
        }

        const size_t size = atomicSize(atomicType);
        if (size == 0 || size > length || requiredLength > length - size)
        {
            return false;
        }
        requiredLength += size;
    }

    return requiredLength <= length;
}

template <typename T>
bool copyAtomic(const uint8_t* frame,
                size_t length,
                const FrameHeader& header,
                int wantedType,
                T& destination)
{
    if ((header.atomics_bitmap & (1UL << wantedType)) == 0)
    {
        return false;
    }

    size_t offset = sizeof(FrameHeader);
    for (int atomicType = 0; atomicType < wantedType; ++atomicType)
    {
        if ((header.atomics_bitmap & (1UL << atomicType)) != 0)
        {
            offset += atomicSize(atomicType);
        }
    }

    if (atomicSize(wantedType) != sizeof(T) ||
        sizeof(T) > length ||
        offset > length - sizeof(T))
    {
        return false;
    }

    memcpy(&destination, frame + offset, sizeof(T));
    return true;
}
} // namespace

bool TelemetryStore::updateFromFrame(const uint8_t* frame, size_t length)
{
    if (frame == nullptr || length < sizeof(FrameHeader))
    {
        return false;
    }

    FrameHeader header{};
    memcpy(&header, frame, sizeof(header));
    if (!validateFrame(header, length))
    {
        return false;
    }

    TelemetryInfo next = latest_;
    next.packet_sequence_number = header.seq;

    gps_atomic_data gps{};
    if (copyAtomic(frame, length, header, AT_GPS_ATOMIC, gps))
    {
        next.gps_latitude_deg = gps.gps_latitude_deg_e7;
        next.gps_longitude_deg = gps.gps_longitude_deg_e7;
        next.gps_altitude_m = gps.gps_altitude_mm;
        next.gps_time_last_update_s = gps.gps_time_last_update_s;
    }

    altitude_atomic_data altitude{};
    if (copyAtomic(frame, length, header, AT_ALTITUDE_ATOMIC, altitude))
    {
        next.baro_altitude_ft = altitude.barometer_altitude_from_pad_ft;
    }

    fc_internal_atomic_data fcInternal{};
    if (copyAtomic(frame, length, header, AT_FC_INTERNAL_ATOMIC, fcInternal))
    {
        next.rssi_dbm = fcInternal.fc_rssi_dBm;
        next.snr_db = fcInternal.fc_snr_dB;
    }

    latest_ = next;
    ++revision_;
    hasTelemetry_ = true;
    return true;
}

const TelemetryInfo& TelemetryStore::latest() const
{
    return latest_;
}

uint32_t TelemetryStore::revision() const
{
    return revision_;
}

bool TelemetryStore::hasTelemetry() const
{
    return hasTelemetry_;
}
