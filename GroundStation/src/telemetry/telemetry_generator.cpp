// AUTO-GENERATED. Do not edit by hand. Generated on: 2026-07-23 22:35
#include "telemetry_generator.h"

#include <cstring>
#include <Arduino.h>

#include "telemetry_packets.h"
#include "frame_builder.h"

static void fillRecovAtomic(recov_atomic_data& x)
{
    x = {};
    x.drogue_armed_SW = false;
    x.drogue_armed_HW = true;
    x.drogue_energized_SW = false;
    x.drogue_energizedGate_HW = true;
    x.drogue_energizedCurrent_HW = false;
    x.drogue_continuity_HW = true;
    x.main_armed_SW = false;
    x.main_armed_HW = true;
    x.main_energized_SW = false;
    x.main_energizedGate_HW = true;
    x.main_energizedCurrent_HW = false;
    x.main_continuity_HW = true;
}

static void fillPropStatesAtomic(prop_states_atomic_data& x)
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
    x.tank_temp_celsius = 0;
    x.vent_temp_celsius = 1;
}

static void fillFlightStageAtomic(flight_stage_atomic_data& x)
{
    x = {};
    x.flight_stage = 0;
}

static void fillFcInternalAtomic(fc_internal_atomic_data& x)
{
    x = {};
    x.fc_pressure_hpa = 0.0f;
    x.fc_time_since_start = 0;
    x.fc_temp_celsius_em2 = 2;
    x.battery_current_draw_mA = 3;
    x.battery_voltage_dV = 1;
    x.fc_rssi_dBm = 2;
    x.fc_snr_dB = 0;
}

static void fillAltitudeAtomic(altitude_atomic_data& x)
{
    x = {};
    x.barometer_altitude_from_pad_ft = 0.1f;
    x.barometer_altitude_from_sea_level_ft = 0.2f;
    x.vertical_speed_ft_per_sec = 0.3f;
}

static void fillAltitudeEventsAtomic(altitude_events_atomic_data& x)
{
    x = {};
    x.apogee_from_ground_ft = 0.4f;
    x.drouge_deployment_from_ground_ft = 0.5f;
    x.main_deployment_from_ground_ft = 0.6f;
}

static void fillAccelerationAtomic(acceleration_atomic_data& x)
{
    x = {};
    x.acceleration_x_mg = 4;
    x.acceleration_y_mg = 5;
    x.acceleration_z_mg = 6;
}

static void fillGyroAtomic(gyro_atomic_data& x)
{
    x = {};
    x.gyro_rate_x_mdeg_per_s = 7;
    x.gyro_rate_y_mdeg_per_s = 8;
    x.gyro_rate_z_mdeg_per_s = 9;
}

static void fillGpsAtomic(gps_atomic_data& x)
{
    x = {};
    x.gps_latitude_deg_e7 = 0.7f;
    x.gps_longitude_deg_e7 = 0.8f;
    x.gps_altitude_mm = 0.9f;
    x.gps_time_last_update_s = 1.0f;
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

static void fillPayloadStatusAtomic(payload_status_atomic_data& x)
{
    x = {};
    x.payload_time_elapsed = 1;
    x.payload_ok = false;
    x.payload_sampling = true;
}

static void fillPayloadDataAtomic(payload_data_atomic_data& x)
{
    x = {};
    x.payload_freq_x = 2;
    x.payload_freq_y = 3;
    x.payload_freq_z = 4;
    x.payload_amp_x = 5;
    x.payload_amp_y = 6;
    x.payload_amp_z = 7;
}

static void fillPayloadAdapter0Atomic(payload_adapter0_atomic_data& x)
{
    x = {};
    x.payload_adapter_0_raw_x = 8;
    x.payload_adapter_0_raw_y = 9;
    x.payload_adapter_0_raw_z = 10;
}

static void fillPayloadAdapter1Atomic(payload_adapter1_atomic_data& x)
{
    x = {};
    x.payload_adapter_1_raw_x = 11;
    x.payload_adapter_1_raw_y = 12;
    x.payload_adapter_1_raw_z = 13;
}

static void fillPayloadAdapter2Atomic(payload_adapter_2_atomic_data& x)
{
    x = {};
    x.payload_adapter_2_raw_x = 14;
    x.payload_adapter_2_raw_y = 15;
    x.payload_adapter_2_raw_z = 16;
}

static void fillPayloadAdapter3Atomic(payload_adapter_3_atomic_data& x)
{
    x = {};
    x.payload_adapter_3_raw_x = 17;
    x.payload_adapter_3_raw_y = 18;
    x.payload_adapter_3_raw_z = 19;
}

static void fillGpsDebugAtomic(gps_debug_atomic_data& x)
{
    x = {};
    x.gps_hacc_mm = 2;
    x.gps_vacc_mm = 3;
    x.gps_ground_speed_mmps = 0;
    x.gps_heading_motion_deg_e5 = 1;
    x.gps_pdop_centi = 20;
    x.gps_fix_type = 3;
    x.gps_satellites = 4;
    x.gps_fix_ok = false;
}

bool buildTelemetryFrame(uint8_t* frameBuf,
                         size_t frameCap,
                         size_t& outFrameLen,
                         uint16_t& inOutSeqNumber,
                         uint8_t flags,
                         uint8_t ack_id)
{
    recov_atomic_data recov_atomic_instance_0{};
    prop_states_atomic_data prop_states_atomic_instance_1{};
    prop_atomic_data prop_atomic_instance_2{};
    flight_stage_atomic_data flight_stage_atomic_instance_3{};
    fc_internal_atomic_data fc_internal_atomic_instance_4{};
    altitude_atomic_data altitude_atomic_instance_5{};
    altitude_events_atomic_data altitude_events_atomic_instance_6{};
    acceleration_atomic_data acceleration_atomic_instance_7{};
    gyro_atomic_data gyro_atomic_instance_8{};
    gps_atomic_data gps_atomic_instance_9{};
    radio_atomic_data radio_atomic_instance_10{};
    sd_atomic_data sd_atomic_instance_11{};
    payload_status_atomic_data payload_status_atomic_instance_12{};
    payload_data_atomic_data payload_data_atomic_instance_13{};
    payload_adapter0_atomic_data payload_adapter0_atomic_instance_14{};
    payload_adapter1_atomic_data payload_adapter1_atomic_instance_15{};
    payload_adapter_2_atomic_data payload_adapter_2_atomic_instance_16{};
    payload_adapter_3_atomic_data payload_adapter_3_atomic_instance_17{};
    gps_debug_atomic_data gps_debug_atomic_instance_18{};

    fillRecovAtomic(recov_atomic_instance_0);
    fillPropStatesAtomic(prop_states_atomic_instance_1);
    fillPropAtomic(prop_atomic_instance_2);
    fillFlightStageAtomic(flight_stage_atomic_instance_3);
    fillFcInternalAtomic(fc_internal_atomic_instance_4);
    fillAltitudeAtomic(altitude_atomic_instance_5);
    fillAltitudeEventsAtomic(altitude_events_atomic_instance_6);
    fillAccelerationAtomic(acceleration_atomic_instance_7);
    fillGyroAtomic(gyro_atomic_instance_8);
    fillGpsAtomic(gps_atomic_instance_9);
    fillRadioAtomic(radio_atomic_instance_10);
    fillSdAtomic(sd_atomic_instance_11);
    fillPayloadStatusAtomic(payload_status_atomic_instance_12);
    fillPayloadDataAtomic(payload_data_atomic_instance_13);
    fillPayloadAdapter0Atomic(payload_adapter0_atomic_instance_14);
    fillPayloadAdapter1Atomic(payload_adapter1_atomic_instance_15);
    fillPayloadAdapter2Atomic(payload_adapter_2_atomic_instance_16);
    fillPayloadAdapter3Atomic(payload_adapter_3_atomic_instance_17);
    fillGpsDebugAtomic(gps_debug_atomic_instance_18);

    FrameBuilder fb(frameBuf, frameCap);

    bool ok = true;
    ok &= fb.addAtomic((int)AT_RECOV_ATOMIC, &recov_atomic_instance_0, sizeof(recov_atomic_instance_0));
    ok &= fb.addAtomic((int)AT_PROP_STATES_ATOMIC, &prop_states_atomic_instance_1, sizeof(prop_states_atomic_instance_1));
    ok &= fb.addAtomic((int)AT_PROP_ATOMIC, &prop_atomic_instance_2, sizeof(prop_atomic_instance_2));
    ok &= fb.addAtomic((int)AT_FLIGHT_STAGE_ATOMIC, &flight_stage_atomic_instance_3, sizeof(flight_stage_atomic_instance_3));
    ok &= fb.addAtomic((int)AT_FC_INTERNAL_ATOMIC, &fc_internal_atomic_instance_4, sizeof(fc_internal_atomic_instance_4));
    ok &= fb.addAtomic((int)AT_ALTITUDE_ATOMIC, &altitude_atomic_instance_5, sizeof(altitude_atomic_instance_5));
    ok &= fb.addAtomic((int)AT_ALTITUDE_EVENTS_ATOMIC, &altitude_events_atomic_instance_6, sizeof(altitude_events_atomic_instance_6));
    ok &= fb.addAtomic((int)AT_ACCELERATION_ATOMIC, &acceleration_atomic_instance_7, sizeof(acceleration_atomic_instance_7));
    ok &= fb.addAtomic((int)AT_GYRO_ATOMIC, &gyro_atomic_instance_8, sizeof(gyro_atomic_instance_8));
    ok &= fb.addAtomic((int)AT_GPS_ATOMIC, &gps_atomic_instance_9, sizeof(gps_atomic_instance_9));
    ok &= fb.addAtomic((int)AT_RADIO_ATOMIC, &radio_atomic_instance_10, sizeof(radio_atomic_instance_10));
    ok &= fb.addAtomic((int)AT_SD_ATOMIC, &sd_atomic_instance_11, sizeof(sd_atomic_instance_11));
    ok &= fb.addAtomic((int)AT_PAYLOAD_STATUS_ATOMIC, &payload_status_atomic_instance_12, sizeof(payload_status_atomic_instance_12));
    ok &= fb.addAtomic((int)AT_PAYLOAD_DATA_ATOMIC, &payload_data_atomic_instance_13, sizeof(payload_data_atomic_instance_13));
    ok &= fb.addAtomic((int)AT_PAYLOAD_ADAPTER0_ATOMIC, &payload_adapter0_atomic_instance_14, sizeof(payload_adapter0_atomic_instance_14));
    ok &= fb.addAtomic((int)AT_PAYLOAD_ADAPTER1_ATOMIC, &payload_adapter1_atomic_instance_15, sizeof(payload_adapter1_atomic_instance_15));
    ok &= fb.addAtomic((int)AT_PAYLOAD_ADAPTER_2_ATOMIC, &payload_adapter_2_atomic_instance_16, sizeof(payload_adapter_2_atomic_instance_16));
    ok &= fb.addAtomic((int)AT_PAYLOAD_ADAPTER_3_ATOMIC, &payload_adapter_3_atomic_instance_17, sizeof(payload_adapter_3_atomic_instance_17));
    ok &= fb.addAtomic((int)AT_GPS_DEBUG_ATOMIC, &gps_debug_atomic_instance_18, sizeof(gps_debug_atomic_instance_18));

    if (!ok)
        return false;

    outFrameLen = fb.finalize(inOutSeqNumber++, flags, ack_id);
    return true;
}
