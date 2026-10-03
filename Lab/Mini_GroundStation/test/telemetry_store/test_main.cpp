#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>

#include "TelemetryStore.h"
#include "frame_builder.h"
#include "telemetry_generator.h"
#include "telemetry_packets.h"

static void near(float actual, float expected, float tolerance = 0.000001f)
{
    assert(std::fabs(actual - expected) < tolerance);
}

int main()
{
    uint8_t frame[512]{};
    size_t length = 0;
    uint16_t sequence = 42;
    TelemetryStore store;
    assert(!store.hasTelemetry());

    // Exercise the actual FC_Mock generator, including every atomic and CTS/ACK.
    for (uint8_t flags : {uint8_t(0), uint8_t(FLAG_CTS), uint8_t(FLAG_ACK), uint8_t(FLAG_NAK)})
    {
        assert(buildTelemetryFrame(frame, sizeof(frame), length, sequence, flags, 7));
        assert(length <= 256); // Mini GS radio buffer / SX126x payload limit.
        assert(store.updateFromFrame(frame, length));
        assert(store.latest().packet_sequence_number == sequence - 1);
        near(store.latest().gps_latitude_deg, 0.7f / 10000000.0f, 1e-12f);
        near(store.latest().gps_longitude_deg, 0.8f / 10000000.0f, 1e-12f);
        near(store.latest().gps_altitude_m, 0.0009f);
        near(store.latest().gps_time_last_update_s, 1.0f);
        near(store.latest().baro_altitude_ft, 0.1f);
        near(store.latest().rssi_dbm, -1.0f);
        near(store.latest().snr_db, 0.0f);
    }

    // Realistic GPS values and fractional, negative RF metrics.
    gps_atomic_data gps{455017000.0f, -735673000.0f, 32400.0f, 1.2f};
    fc_internal_atomic_data fc{};
    fc.fc_rssi_dBm = 175;
    fc.fc_snr_dB = -9;
    FrameBuilder realistic(frame, sizeof(frame));
    assert(realistic.addAtomic(AT_GPS_ATOMIC, &gps, sizeof(gps)));
    assert(realistic.addAtomic(AT_FC_INTERNAL_ATOMIC, &fc, sizeof(fc)));
    length = realistic.finalize(100, FLAG_CTS, 0);
    assert(store.updateFromFrame(frame, length));
    near(store.latest().gps_latitude_deg, 45.5017f, 0.00001f);
    near(store.latest().gps_longitude_deg, -73.5673f, 0.00001f);
    near(store.latest().gps_altitude_m, 32.4f, 0.00001f);
    near(store.latest().rssi_dbm, -87.5f);
    near(store.latest().snr_db, -2.25f);

    // A partial update must preserve the omitted GPS and FC atomics.
    altitude_atomic_data altitude{123.4f, 200.0f, 5.0f};
    FrameBuilder partial(frame, sizeof(frame));
    assert(partial.addAtomic(AT_ALTITUDE_ATOMIC, &altitude, sizeof(altitude)));
    length = partial.finalize(101, 0, 0);
    assert(store.updateFromFrame(frame, length));
    near(store.latest().baro_altitude_ft, 123.4f, 0.00001f);
    near(store.latest().gps_altitude_m, 32.4f, 0.00001f);
    near(store.latest().rssi_dbm, -87.5f);

    const uint32_t revision = store.revision();
    assert(!store.updateFromFrame(nullptr, length));
    assert(!store.updateFromFrame(frame, sizeof(FrameHeader) - 1));
    assert(!store.updateFromFrame(frame, length - 1));
    assert(!store.updateFromFrame(frame, length + 1));
    FrameHeader invalid{102, 0, 0, 1u << AT_TOTAL};
    std::memcpy(frame, &invalid, sizeof(invalid));
    assert(!store.updateFromFrame(frame, sizeof(invalid)));
    invalid.atomics_bitmap = 0;
    std::memcpy(frame, &invalid, sizeof(invalid));
    assert(!store.updateFromFrame(frame, sizeof(invalid)));
    assert(store.revision() == revision);
    assert(store.latest().packet_sequence_number == 101);
    std::cout << "TelemetryStore: mock frames, calibration, partial updates and invalid frames passed\n";
}
