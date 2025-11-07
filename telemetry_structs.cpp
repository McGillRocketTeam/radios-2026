#include <cstdint>
#include <string>

struct prop_atomic {
    uint16_t cc_pressure;
    uint16_t tank_pressure;
    uint16_t tank_temp;
    uint8_t vent_temp;
    bool mov_hall_state;
};

struct valve_atomic {
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
};

struct test_atomic {
    uint16_t tank_temp;
    bool mov_hall_state;
    bool fdov_continuity_HW;
};

