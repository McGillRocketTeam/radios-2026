// AUTO-GENERATED. Do not edit by hand.
#include "telemetry_generator.h"

#include <cstring>
#include <Arduino.h>

#include "telemetry_packets.h"
#include "frame_builder.h"

static void fillStatesAtomic(states_atomic_data& x)
{
    x = {};
    x.mov_hall_state = false;
    x.fdov_armed_SW = false;
    x.fdov_armed_HW = false;
    x.fdov_energized_SW = false;
    x.fdov_energizedGate_HW = false;
    x.fdov_energizedCurrent_HW = false;
    x.fdov_continuity_HW = false;
    x.mov_armed_SW = false;
    x.mov_armed_HW = false;
    x.mov_energized_SW = false;
    x.mov_energizedGate_HW = false;
    x.mov_energizedCurrent_HW = false;
    x.mov_continuity_HW = false;
    x.drogue_armed_SW = false;
    x.drogue_armed_HW = false;
    x.drogue_energized_SW = false;
    x.drogue_energizedGate_HW = false;
    x.drogue_energizedCurrent_HW = false;
    x.drogue_continuity_HW = false;
    x.main_armed_SW = false;
    x.main_armed_HW = false;
    x.main_energized_SW = false;
    x.main_energizedGate_HW = false;
    x.main_energizedCurrent_HW = false;
    x.main_continuity_HW = false;
    x.prop_energized_electric = false;
    x.vent_armed_SW = false;
    x.vent_armed_HW = false;
    x.vent_energized_SW = false;
    x.vent_energizedGate_HW = false;
    x.vent_energizedCurrent_HW = false;
    x.vent_continuity_HW = false;
}

static void fillPropAtomic(prop_atomic_data& x)
{
    x = {};
    x.cc_pressure = 0;
    x.tank_pressure = 0;
    x.tank_temp = 0;
    x.vent_temp = 0;
}

static void fillFlightAtomic(flight_atomic_data& x)
{
    x = {};
    x.flight_stage = 0;
    x.barometer_altitude_from_pad = 0.0f;
    x.barometer_altitude_from_sea_level = 0.0f;
    x.fc_pressure = 0.0f;
    x.apogee_from_ground = 0.0f;
    x.fc_temp = 0;
    x.gps_latitude = 0.0f;
    x.gps_longitude = 0.0f;
    x.gps_altitude = 0.0f;
    x.gps_time_last_update = 0.0f;
    x.vertical_speed = 0.0f;
    x.acceleration_x = 0;
    x.acceleration_y = 0;
    x.acceleration_z = 0;
    x.gyro_rate_x = 0;
    x.gyro_rate_y = 0;
    x.gyro_rate_z = 0;
    x.fc_rssi = 0;
    x.fc_snr = 0;
    x.battery_voltage = 0;
    x.battery_current_draw = 0;
}

static void fillRadioAtomic(radio_atomic_data& x)
{
    x = {};
    std::strncpy(x.call_sign, "", sizeof(x.call_sign));
    x.call_sign[sizeof(x.call_sign) - 1] = '\0';
}

static void fillSdAtomic(sd_atomic_data& x)
{
    x = {};
    x.sd_card_file_open = false;
    x.sd_card_deletion_armed = false;
}

bool buildTelemetryFrame(uint8_t* frameBuf,
                         size_t frameCap,
                         size_t& outFrameLen,
                         uint16_t& inOutSeqNumber,
                         uint8_t flags,
                         uint8_t ack_id)
{
    states_atomic_data states_atomic_instance_0{};
    prop_atomic_data prop_atomic_instance_1{};
    flight_atomic_data flight_atomic_instance_2{};
    radio_atomic_data radio_atomic_instance_3{};
    sd_atomic_data sd_atomic_instance_4{};

    fillStatesAtomic(states_atomic_instance_0);
    fillPropAtomic(prop_atomic_instance_1);
    fillFlightAtomic(flight_atomic_instance_2);
    fillRadioAtomic(radio_atomic_instance_3);
    fillSdAtomic(sd_atomic_instance_4);

    FrameBuilder fb(frameBuf, frameCap);

    bool ok = true;
    ok &= fb.addAtomic((int)AT_STATES_ATOMIC, &states_atomic_instance_0, sizeof(states_atomic_instance_0));
    ok &= fb.addAtomic((int)AT_PROP_ATOMIC, &prop_atomic_instance_1, sizeof(prop_atomic_instance_1));
    ok &= fb.addAtomic((int)AT_FLIGHT_ATOMIC, &flight_atomic_instance_2, sizeof(flight_atomic_instance_2));
    ok &= fb.addAtomic((int)AT_RADIO_ATOMIC, &radio_atomic_instance_3, sizeof(radio_atomic_instance_3));
    ok &= fb.addAtomic((int)AT_SD_ATOMIC, &sd_atomic_instance_4, sizeof(sd_atomic_instance_4));

    if (!ok)
        return false;

    outFrameLen = fb.finalize(inOutSeqNumber++, flags, ack_id);
    return true;
}
