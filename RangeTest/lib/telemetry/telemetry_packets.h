// AUTO-GENERATED. Do not edit by hand.
#pragma once
#include <cstdint>

// ---------- AtomicType Enum ----------
enum AtomicType {
    AT_STATES_ATOMIC        = 0,  // bit 0
    AT_PROP_ATOMIC          = 1,  // bit 1
    AT_FLIGHT_ATOMIC        = 2,  // bit 2
    AT_RADIO_ATOMIC         = 3,  // bit 3
    AT_TOTAL                             // total count
};

// ---------- states_atomic atomic ----------
typedef struct __attribute__((__packed__)) states_atomic_data
{
    bool mov_hall_state : 1;
    bool fdov_armed_SW : 1;
    bool fdov_armed_HW : 1;
    bool fdov_energized_SW : 1;
    bool fdov_energizedGate_HW : 1;
    bool fdov_energizedCurrent_HW : 1;
    bool fdov_continuity_HW : 1;
    bool mov_armed_SW : 1;
    bool mov_armed_HW : 1;
    bool mov_energized_SW : 1;
    bool mov_energizedGate_HW : 1;
    bool mov_energizedCurrent_HW : 1;
    bool mov_continuity_HW : 1;
    bool pilot_armed_SW : 1;
    bool pilot_armed_HW : 1;
    bool pilot_energized_SW : 1;
    bool pilot_energizedGate_HW : 1;
    bool pilot_energizedCurrent_HW : 1;
    bool pilot_continuity_HW : 1;
    bool ring_armed_SW : 1;
    bool ring_armed_HW : 1;
    bool ring_energized_SW : 1;
    bool ring_energizedGate_HW : 1;
    bool ring_energizedCurrent_HW : 1;
    bool ring_continuity_HW : 1;
    bool prop_energized_electric : 1;
    bool vent_armed_SW : 1;
    bool vent_armed_HW : 1;
    bool vent_energized_SW : 1;
    bool vent_energizedGate_HW : 1;
    bool vent_energizedCurrent_HW : 1;
    bool vent_continuity_HW : 1;
} states_atomic_data;

typedef union states_atomic_packet {
    states_atomic_data data;
    uint8_t bytes[sizeof(states_atomic_data)];
} states_atomic_packet;

// ---------- prop_atomic atomic ----------
typedef struct __attribute__((__packed__)) prop_atomic_data
{
    uint16_t cc_pressure;
    uint16_t tank_pressure;
    uint16_t tank_temp;
    uint8_t vent_temp;
} prop_atomic_data;

typedef union prop_atomic_packet {
    prop_atomic_data data;
    uint8_t bytes[sizeof(prop_atomic_data)];
} prop_atomic_packet;

// ---------- flight_atomic atomic ----------
typedef struct __attribute__((__packed__)) flight_atomic_data
{
    uint8_t flight_stage;
    float altimeter_altitude;
    float altitude_from_sea_level;
    float apogee_from_ground;
    float atm_pressure;
    float barometer_altitude;
    float atm_temp;
    float gps_latitude;
    float gps_longitude;
    float gps_altitude;
    float gps_time_last_update;
    float vertical_speed;
    int16_t acceleration_x;
    int16_t acceleration_y;
    int16_t acceleration_z;
    uint16_t angle_yaw;
    uint16_t angle_pitch;
    uint16_t angle_roll;
    uint16_t fc_rssi;
    int8_t fc_snr;
} flight_atomic_data;

typedef union flight_atomic_packet {
    flight_atomic_data data;
    uint8_t bytes[sizeof(flight_atomic_data)];
} flight_atomic_packet;

// ---------- radio_atomic atomic ----------
typedef struct __attribute__((__packed__)) radio_atomic_data
{
    char call_sign[6];
} radio_atomic_data;

typedef union radio_atomic_packet {
    radio_atomic_data data;
    uint8_t bytes[sizeof(radio_atomic_data)];
} radio_atomic_packet;

// ---------- Atomic Size Catalog (declaration) ----------
extern const uint16_t AT_SIZE[AT_TOTAL];
