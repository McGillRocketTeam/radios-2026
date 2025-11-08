// AUTO-GENERATED. Do not edit by hand.
#ifndef TELEMETRY_PACKETS_H
#define TELEMETRY_PACKETS_H

#include <cstdint>

// ---------- AtomicType Enum ----------
enum AtomicType {
    AT_PROP_ATOMIC          = 0,  // bit 0
    AT_VALVE_ATOMIC         = 1,  // bit 1
    AT_TEST_ATOMIC          = 2,  // bit 2
    AT_TOTAL                             // total count
};

// ---------- prop_atomic atomic ----------
typedef struct __attribute__((__packed__)) prop_atomic_data
{
    uint16_t cc_pressure;
    uint16_t tank_pressure;
    uint16_t tank_temp;
    uint8_t vent_temp;
    bool mov_hall_state;
} prop_atomic_data;

typedef union prop_atomic_packet {
    prop_atomic_data data;
    uint8_t bytes[sizeof(prop_atomic_data)];
} prop_atomic_packet;

// ---------- valve_atomic atomic ----------
typedef struct __attribute__((__packed__)) valve_atomic_data
{
    bool fdov_armed_SW;
    bool fdov_armed_HW;
    bool fdov_energized_SW;
    bool fdov_energizedGate_HW;
    bool fdov_energizedCurrent_HW;
    bool fdov_continuity_HW;
    bool mov_armed_SW;
    bool mov_armed_HW;
    bool mov_energized_SW;
    bool mov_energizedGate_HW;
    bool mov_energizedCurrent_HW;
    bool mov_continuity_HW;
} valve_atomic_data;

typedef union valve_atomic_packet {
    valve_atomic_data data;
    uint8_t bytes[sizeof(valve_atomic_data)];
} valve_atomic_packet;

// ---------- test_atomic atomic ----------
typedef struct __attribute__((__packed__)) test_atomic_data
{
    bool mov_hall_state;
    bool fdov_continuity_HW;
    uint16_t tank_temp;
} test_atomic_data;

typedef union test_atomic_packet {
    test_atomic_data data;
    uint8_t bytes[sizeof(test_atomic_data)];
} test_atomic_packet;

// ---------- Atomic Size Catalog ----------
static const uint16_t AT_SIZE[AT_TOTAL] = {
    /*0*/ sizeof(prop_atomic_data),
    /*1*/ sizeof(valve_atomic_data),
    /*2*/ sizeof(test_atomic_data),
};

#endif // TELEMETRY_PACKETS_H
