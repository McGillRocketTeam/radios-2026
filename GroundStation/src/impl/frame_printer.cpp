// AUTO-GENERATED. Do not edit by hand.
#include "frame_printer.h"
#include <Arduino.h>

#define PRINT_FIELD(p, field) { \
  Serial.print(F(#field ": ")); \
  Serial.println(p->field); \
}

static inline void printBoolLine(const __FlashStringHelper* label, bool b) {
  Serial.print(label);
  Serial.println(b ? F("true") : F("false"));
}

const __FlashStringHelper* atomicTypeName(AtomicType at) {
  switch (at) {
    case AT_STATES_ATOMIC: return F("states_atomic");
    case AT_PROP_ATOMIC: return F("prop_atomic");
    case AT_FLIGHT_ATOMIC: return F("flight_atomic");
    case AT_RADIO_ATOMIC: return F("radio_atomic");
    default: return F("unknown_atomic");
  }
}

// ---------------- states_atomic ----------------
void printStates_atomicAtomic(const states_atomic_data* p) {
  if (!p) return;
  Serial.println(F("states_atomic {"));
  PRINT_FIELD(p, mov_hall_state);
  PRINT_FIELD(p, fdov_armed_SW);
  PRINT_FIELD(p, fdov_armed_HW);
  PRINT_FIELD(p, fdov_energized_SW);
  PRINT_FIELD(p, fdov_energizedGate_HW);
  PRINT_FIELD(p, fdov_energizedCurrent_HW);
  PRINT_FIELD(p, fdov_continuity_HW);
  PRINT_FIELD(p, mov_armed_SW);
  PRINT_FIELD(p, mov_armed_HW);
  PRINT_FIELD(p, mov_energized_SW);
  PRINT_FIELD(p, mov_energizedGate_HW);
  PRINT_FIELD(p, mov_energizedCurrent_HW);
  PRINT_FIELD(p, mov_continuity_HW);
  PRINT_FIELD(p, drogue_armed_SW);
  PRINT_FIELD(p, drogue_armed_HW);
  PRINT_FIELD(p, drogue_energized_SW);
  PRINT_FIELD(p, drogue_energizedGate_HW);
  PRINT_FIELD(p, drogue_energizedCurrent_HW);
  PRINT_FIELD(p, drogue_continuity_HW);
  PRINT_FIELD(p, main_armed_SW);
  PRINT_FIELD(p, main_armed_HW);
  PRINT_FIELD(p, main_energized_SW);
  PRINT_FIELD(p, main_energizedGate_HW);
  PRINT_FIELD(p, main_energizedCurrent_HW);
  PRINT_FIELD(p, main_continuity_HW);
  PRINT_FIELD(p, prop_energized_electric);
  PRINT_FIELD(p, vent_armed_SW);
  PRINT_FIELD(p, vent_armed_HW);
  PRINT_FIELD(p, vent_energized_SW);
  PRINT_FIELD(p, vent_energizedGate_HW);
  PRINT_FIELD(p, vent_energizedCurrent_HW);
  PRINT_FIELD(p, vent_continuity_HW);
  Serial.println(F("}"));
}

// ---------------- prop_atomic ----------------
void printProp_atomicAtomic(const prop_atomic_data* p) {
  if (!p) return;
  Serial.println(F("prop_atomic {"));
  PRINT_FIELD(p, cc_pressure);
  PRINT_FIELD(p, tank_pressure);
  PRINT_FIELD(p, tank_temp);
  PRINT_FIELD(p, vent_temp);
  Serial.println(F("}"));
}

// ---------------- flight_atomic ----------------
void printFlight_atomicAtomic(const flight_atomic_data* p) {
  if (!p) return;
  Serial.println(F("flight_atomic {"));
  PRINT_FIELD(p, flight_stage);
  PRINT_FIELD(p, barometer_altitude_from_pad);
  PRINT_FIELD(p, barometer_altitude_from_sea_level);
  PRINT_FIELD(p, fc_pressure);
  PRINT_FIELD(p, apogee_from_ground);
  PRINT_FIELD(p, fc_temp);
  PRINT_FIELD(p, gps_latitude);
  PRINT_FIELD(p, gps_longitude);
  PRINT_FIELD(p, gps_altitude);
  PRINT_FIELD(p, gps_time_last_update);
  PRINT_FIELD(p, vertical_speed);
  PRINT_FIELD(p, acceleration_x);
  PRINT_FIELD(p, acceleration_y);
  PRINT_FIELD(p, acceleration_z);
  PRINT_FIELD(p, gyro_rate_x);
  PRINT_FIELD(p, gyro_rate_y);
  PRINT_FIELD(p, gyro_rate_z);
  PRINT_FIELD(p, fc_rssi);
  PRINT_FIELD(p, fc_snr);
  PRINT_FIELD(p, battery_voltage);
  PRINT_FIELD(p, battery_current_draw);
  Serial.println(F("}"));
}

// ---------------- radio_atomic ----------------
void printRadio_atomicAtomic(const radio_atomic_data* p) {
  if (!p) return;
  Serial.println(F("radio_atomic {"));
  PRINT_FIELD(p, call_sign);
  Serial.println(F("}"));
}

void printAtomic(const FrameView& view, AtomicType at) {
  switch (at) {
    case AT_STATES_ATOMIC: {
      const auto* p = view.atomicAs<states_atomic_data>(AT_STATES_ATOMIC);
      printStates_atomicAtomic(p);
      break;
    }
    case AT_PROP_ATOMIC: {
      const auto* p = view.atomicAs<prop_atomic_data>(AT_PROP_ATOMIC);
      printProp_atomicAtomic(p);
      break;
    }
    case AT_FLIGHT_ATOMIC: {
      const auto* p = view.atomicAs<flight_atomic_data>(AT_FLIGHT_ATOMIC);
      printFlight_atomicAtomic(p);
      break;
    }
    case AT_RADIO_ATOMIC: {
      const auto* p = view.atomicAs<radio_atomic_data>(AT_RADIO_ATOMIC);
      printRadio_atomicAtomic(p);
      break;
    }
    default: Serial.print(F("Unknown atomic type: ")); Serial.println(static_cast<int>(at)); break;
  }
}

void printAtomics(const FrameView& view) {
  Serial.println(F("\n--- Atomics (decoded) ---"));
  for (int i = 0; i < AT_TOTAL; ++i) {
    auto at = static_cast<AtomicType>(i);
    if (view.hasAtomic(at)) {
      printAtomic(view, at);
    }
  }
}