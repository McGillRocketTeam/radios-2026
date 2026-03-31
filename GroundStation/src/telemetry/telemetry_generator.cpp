// AUTO-GENERATED. Do not edit by hand. Generated on: 2026-03-31 16:18
#include "telemetry_generator.h"

#include <cstring>
#include <Arduino.h>

#include "telemetry_packets.h"
#include "frame_builder.h"

static void fillStatesAtomic(states_atomic_data& x)
{
    x = {};
    x.mov_hall_state = false;
    x.fdov_armed_SW = true;
    x.fdov_armed_HW = false;
    x.fdov_energized_SW = true;
    x.fdov_energizedGate_HW = false;
    x.fdov_energizedCurrent_HW = true;
    x.fdov_continuity_HW = false;
    x.mov_armed_SW = true;
    x.mov_armed_HW = false;
    x.mov_energized_SW = true;
    x.mov_energizedGate_HW = false;
    x.mov_energizedCurrent_HW = true;
    x.mov_continuity_HW = false;
    x.drogue_armed_SW = true;
    x.drogue_armed_HW = false;
    x.drogue_energized_SW = true;
    x.drogue_energizedGate_HW = false;
    x.drogue_energizedCurrent_HW = true;
    x.drogue_continuity_HW = false;
    x.main_armed_SW = true;
    x.main_armed_HW = false;
    x.main_energized_SW = true;
    x.main_energizedGate_HW = false;
    x.main_energizedCurrent_HW = true;
    x.main_continuity_HW = false;
    x.prop_energized_electric = true;
    x.vent_armed_SW = false;
    x.vent_armed_HW = true;
    x.vent_energized_SW = false;
    x.vent_energizedGate_HW = true;
    x.vent_energizedCurrent_HW = false;
    x.vent_continuity_HW = true;
}

static void fillPropAtomic(prop_atomic_data& x)
{
    x = {};
    x.cc_pressure = 0;
    x.tank_pressure = 1;
    x.tank_temp_celsius = 2;
    x.vent_temp_celsius = 0;
}

static void fillFlightAtomic(flight_atomic_data& x)
{
    x = {};
    x.flight_stage = 1;
    x.barometer_altitude_from_pad_ft = 0.0f;
    x.barometer_altitude_from_sea_level_ft = 0.1f;
    x.fc_pressure_hpa = 0.2f;
    x.apogee_from_ground_ft = 0.3f;
    x.fc_temp_celsius_em2 = 0;
    x.gps_latitude_deg_e7 = 0.4f;
    x.gps_longitude_deg_e7 = 0.5f;
    x.gps_altitude_mm = 0.6f;
    x.gps_time_last_update_s = 0.7f;
    x.vertical_speed_ft_per_sec = 0.8f;
    x.acceleration_x_mg = 1;
    x.acceleration_y_mg = 2;
    x.acceleration_z_mg = 3;
    x.gyro_rate_x_mdeg_per_s = 4;
    x.gyro_rate_y_mdeg_per_s = 5;
    x.gyro_rate_z_mdeg_per_s = 6;
    x.fc_rssi_dBm = 2;
    x.fc_snr_dB = 0;
    x.battery_voltage_dV = 3;
    x.battery_current_draw_mA = 7;
}

static void fillRadioAtomic(radio_atomic_data& x)
{
    x = {};
    std::strncpy(x.call_sign, "str0", sizeof(x.call_sign));
    x.call_sign[sizeof(x.call_sign) - 1] = '\0';
}

static void fillSdAtomic(sd_atomic_data& x)
{
    x = {};
    x.sd_card_file_open = false;
    x.sd_card_deletion_armed = true;
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
