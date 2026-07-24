// AUTO-GENERATED. Do not edit by hand. Generated on: 2026-07-23 22:35
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
    case AT_RECOV_ATOMIC: return F("recov_atomic");
    case AT_PROP_STATES_ATOMIC: return F("prop_states_atomic");
    case AT_PROP_ATOMIC: return F("prop_atomic");
    case AT_FLIGHT_STAGE_ATOMIC: return F("flight_stage_atomic");
    case AT_FC_INTERNAL_ATOMIC: return F("fc_internal_atomic");
    case AT_ALTITUDE_ATOMIC: return F("altitude_atomic");
    case AT_ALTITUDE_EVENTS_ATOMIC: return F("altitude_events_atomic");
    case AT_ACCELERATION_ATOMIC: return F("acceleration_atomic");
    case AT_GYRO_ATOMIC: return F("gyro_atomic");
    case AT_GPS_ATOMIC: return F("gps_atomic");
    case AT_RADIO_ATOMIC: return F("radio_atomic");
    case AT_SD_ATOMIC: return F("sd_atomic");
    case AT_PAYLOAD_STATUS_ATOMIC: return F("payload_status_atomic");
    case AT_PAYLOAD_DATA_ATOMIC: return F("payload_data_atomic");
    case AT_PAYLOAD_ADAPTER0_ATOMIC: return F("payload_adapter0_atomic");
    case AT_PAYLOAD_ADAPTER1_ATOMIC: return F("payload_adapter1_atomic");
    case AT_PAYLOAD_ADAPTER_2_ATOMIC: return F("payload_adapter_2_atomic");
    case AT_PAYLOAD_ADAPTER_3_ATOMIC: return F("payload_adapter_3_atomic");
    case AT_GPS_DEBUG_ATOMIC: return F("gps_debug_atomic");
    default: return F("unknown_atomic");
  }
}

// ---------------- recov_atomic ----------------
void printRecov_atomicAtomic(const recov_atomic_data* p) {
  if (!p) return;
  Serial.println(F("recov_atomic {"));
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
  Serial.println(F("}"));
}

// ---------------- prop_states_atomic ----------------
void printProp_states_atomicAtomic(const prop_states_atomic_data* p) {
  if (!p) return;
  Serial.println(F("prop_states_atomic {"));
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
  PRINT_FIELD(p, tank_temp_celsius);
  PRINT_FIELD(p, vent_temp_celsius);
  Serial.println(F("}"));
}

// ---------------- flight_stage_atomic ----------------
void printFlight_stage_atomicAtomic(const flight_stage_atomic_data* p) {
  if (!p) return;
  Serial.println(F("flight_stage_atomic {"));
  PRINT_FIELD(p, flight_stage);
  Serial.println(F("}"));
}

// ---------------- fc_internal_atomic ----------------
void printFc_internal_atomicAtomic(const fc_internal_atomic_data* p) {
  if (!p) return;
  Serial.println(F("fc_internal_atomic {"));
  PRINT_FIELD(p, fc_pressure_hpa);
  PRINT_FIELD(p, fc_time_since_start);
  PRINT_FIELD(p, fc_temp_celsius_em2);
  PRINT_FIELD(p, battery_current_draw_mA);
  PRINT_FIELD(p, battery_voltage_dV);
  PRINT_FIELD(p, fc_rssi_dBm);
  PRINT_FIELD(p, fc_snr_dB);
  Serial.println(F("}"));
}

// ---------------- altitude_atomic ----------------
void printAltitude_atomicAtomic(const altitude_atomic_data* p) {
  if (!p) return;
  Serial.println(F("altitude_atomic {"));
  PRINT_FIELD(p, barometer_altitude_from_pad_ft);
  PRINT_FIELD(p, barometer_altitude_from_sea_level_ft);
  PRINT_FIELD(p, vertical_speed_ft_per_sec);
  Serial.println(F("}"));
}

// ---------------- altitude_events_atomic ----------------
void printAltitude_events_atomicAtomic(const altitude_events_atomic_data* p) {
  if (!p) return;
  Serial.println(F("altitude_events_atomic {"));
  PRINT_FIELD(p, apogee_from_ground_ft);
  PRINT_FIELD(p, drouge_deployment_from_ground_ft);
  PRINT_FIELD(p, main_deployment_from_ground_ft);
  Serial.println(F("}"));
}

// ---------------- acceleration_atomic ----------------
void printAcceleration_atomicAtomic(const acceleration_atomic_data* p) {
  if (!p) return;
  Serial.println(F("acceleration_atomic {"));
  PRINT_FIELD(p, acceleration_x_mg);
  PRINT_FIELD(p, acceleration_y_mg);
  PRINT_FIELD(p, acceleration_z_mg);
  Serial.println(F("}"));
}

// ---------------- gyro_atomic ----------------
void printGyro_atomicAtomic(const gyro_atomic_data* p) {
  if (!p) return;
  Serial.println(F("gyro_atomic {"));
  PRINT_FIELD(p, gyro_rate_x_mdeg_per_s);
  PRINT_FIELD(p, gyro_rate_y_mdeg_per_s);
  PRINT_FIELD(p, gyro_rate_z_mdeg_per_s);
  Serial.println(F("}"));
}

// ---------------- gps_atomic ----------------
void printGps_atomicAtomic(const gps_atomic_data* p) {
  if (!p) return;
  Serial.println(F("gps_atomic {"));
  PRINT_FIELD(p, gps_latitude_deg_e7);
  PRINT_FIELD(p, gps_longitude_deg_e7);
  PRINT_FIELD(p, gps_altitude_mm);
  PRINT_FIELD(p, gps_time_last_update_s);
  Serial.println(F("}"));
}

// ---------------- radio_atomic ----------------
void printRadio_atomicAtomic(const radio_atomic_data* p) {
  if (!p) return;
  Serial.println(F("radio_atomic {"));
  PRINT_FIELD(p, call_sign);
  Serial.println(F("}"));
}

// ---------------- sd_atomic ----------------
void printSd_atomicAtomic(const sd_atomic_data* p) {
  if (!p) return;
  Serial.println(F("sd_atomic {"));
  PRINT_FIELD(p, sd_card_file_open);
  PRINT_FIELD(p, sd_card_deletion_armed);
  Serial.println(F("}"));
}

// ---------------- payload_status_atomic ----------------
void printPayload_status_atomicAtomic(const payload_status_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_status_atomic {"));
  PRINT_FIELD(p, payload_time_elapsed);
  PRINT_FIELD(p, payload_ok);
  PRINT_FIELD(p, payload_sampling);
  Serial.println(F("}"));
}

// ---------------- payload_data_atomic ----------------
void printPayload_data_atomicAtomic(const payload_data_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_data_atomic {"));
  PRINT_FIELD(p, payload_freq_x);
  PRINT_FIELD(p, payload_freq_y);
  PRINT_FIELD(p, payload_freq_z);
  PRINT_FIELD(p, payload_amp_x);
  PRINT_FIELD(p, payload_amp_y);
  PRINT_FIELD(p, payload_amp_z);
  Serial.println(F("}"));
}

// ---------------- payload_adapter0_atomic ----------------
void printPayload_adapter0_atomicAtomic(const payload_adapter0_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_adapter0_atomic {"));
  PRINT_FIELD(p, payload_adapter_0_raw_x);
  PRINT_FIELD(p, payload_adapter_0_raw_y);
  PRINT_FIELD(p, payload_adapter_0_raw_z);
  Serial.println(F("}"));
}

// ---------------- payload_adapter1_atomic ----------------
void printPayload_adapter1_atomicAtomic(const payload_adapter1_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_adapter1_atomic {"));
  PRINT_FIELD(p, payload_adapter_1_raw_x);
  PRINT_FIELD(p, payload_adapter_1_raw_y);
  PRINT_FIELD(p, payload_adapter_1_raw_z);
  Serial.println(F("}"));
}

// ---------------- payload_adapter_2_atomic ----------------
void printPayload_adapter_2_atomicAtomic(const payload_adapter_2_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_adapter_2_atomic {"));
  PRINT_FIELD(p, payload_adapter_2_raw_x);
  PRINT_FIELD(p, payload_adapter_2_raw_y);
  PRINT_FIELD(p, payload_adapter_2_raw_z);
  Serial.println(F("}"));
}

// ---------------- payload_adapter_3_atomic ----------------
void printPayload_adapter_3_atomicAtomic(const payload_adapter_3_atomic_data* p) {
  if (!p) return;
  Serial.println(F("payload_adapter_3_atomic {"));
  PRINT_FIELD(p, payload_adapter_3_raw_x);
  PRINT_FIELD(p, payload_adapter_3_raw_y);
  PRINT_FIELD(p, payload_adapter_3_raw_z);
  Serial.println(F("}"));
}

// ---------------- gps_debug_atomic ----------------
void printGps_debug_atomicAtomic(const gps_debug_atomic_data* p) {
  if (!p) return;
  Serial.println(F("gps_debug_atomic {"));
  PRINT_FIELD(p, gps_hacc_mm);
  PRINT_FIELD(p, gps_vacc_mm);
  PRINT_FIELD(p, gps_ground_speed_mmps);
  PRINT_FIELD(p, gps_heading_motion_deg_e5);
  PRINT_FIELD(p, gps_pdop_centi);
  PRINT_FIELD(p, gps_fix_type);
  PRINT_FIELD(p, gps_satellites);
  PRINT_FIELD(p, gps_fix_ok);
  Serial.println(F("}"));
}

void printAtomic(const FrameView& view, AtomicType at) {
  switch (at) {
    case AT_RECOV_ATOMIC: {
      const auto* p = view.atomicAs<recov_atomic_data>(AT_RECOV_ATOMIC);
      printRecov_atomicAtomic(p);
      break;
    }
    case AT_PROP_STATES_ATOMIC: {
      const auto* p = view.atomicAs<prop_states_atomic_data>(AT_PROP_STATES_ATOMIC);
      printProp_states_atomicAtomic(p);
      break;
    }
    case AT_PROP_ATOMIC: {
      const auto* p = view.atomicAs<prop_atomic_data>(AT_PROP_ATOMIC);
      printProp_atomicAtomic(p);
      break;
    }
    case AT_FLIGHT_STAGE_ATOMIC: {
      const auto* p = view.atomicAs<flight_stage_atomic_data>(AT_FLIGHT_STAGE_ATOMIC);
      printFlight_stage_atomicAtomic(p);
      break;
    }
    case AT_FC_INTERNAL_ATOMIC: {
      const auto* p = view.atomicAs<fc_internal_atomic_data>(AT_FC_INTERNAL_ATOMIC);
      printFc_internal_atomicAtomic(p);
      break;
    }
    case AT_ALTITUDE_ATOMIC: {
      const auto* p = view.atomicAs<altitude_atomic_data>(AT_ALTITUDE_ATOMIC);
      printAltitude_atomicAtomic(p);
      break;
    }
    case AT_ALTITUDE_EVENTS_ATOMIC: {
      const auto* p = view.atomicAs<altitude_events_atomic_data>(AT_ALTITUDE_EVENTS_ATOMIC);
      printAltitude_events_atomicAtomic(p);
      break;
    }
    case AT_ACCELERATION_ATOMIC: {
      const auto* p = view.atomicAs<acceleration_atomic_data>(AT_ACCELERATION_ATOMIC);
      printAcceleration_atomicAtomic(p);
      break;
    }
    case AT_GYRO_ATOMIC: {
      const auto* p = view.atomicAs<gyro_atomic_data>(AT_GYRO_ATOMIC);
      printGyro_atomicAtomic(p);
      break;
    }
    case AT_GPS_ATOMIC: {
      const auto* p = view.atomicAs<gps_atomic_data>(AT_GPS_ATOMIC);
      printGps_atomicAtomic(p);
      break;
    }
    case AT_RADIO_ATOMIC: {
      const auto* p = view.atomicAs<radio_atomic_data>(AT_RADIO_ATOMIC);
      printRadio_atomicAtomic(p);
      break;
    }
    case AT_SD_ATOMIC: {
      const auto* p = view.atomicAs<sd_atomic_data>(AT_SD_ATOMIC);
      printSd_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_STATUS_ATOMIC: {
      const auto* p = view.atomicAs<payload_status_atomic_data>(AT_PAYLOAD_STATUS_ATOMIC);
      printPayload_status_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_DATA_ATOMIC: {
      const auto* p = view.atomicAs<payload_data_atomic_data>(AT_PAYLOAD_DATA_ATOMIC);
      printPayload_data_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_ADAPTER0_ATOMIC: {
      const auto* p = view.atomicAs<payload_adapter0_atomic_data>(AT_PAYLOAD_ADAPTER0_ATOMIC);
      printPayload_adapter0_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_ADAPTER1_ATOMIC: {
      const auto* p = view.atomicAs<payload_adapter1_atomic_data>(AT_PAYLOAD_ADAPTER1_ATOMIC);
      printPayload_adapter1_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_ADAPTER_2_ATOMIC: {
      const auto* p = view.atomicAs<payload_adapter_2_atomic_data>(AT_PAYLOAD_ADAPTER_2_ATOMIC);
      printPayload_adapter_2_atomicAtomic(p);
      break;
    }
    case AT_PAYLOAD_ADAPTER_3_ATOMIC: {
      const auto* p = view.atomicAs<payload_adapter_3_atomic_data>(AT_PAYLOAD_ADAPTER_3_ATOMIC);
      printPayload_adapter_3_atomicAtomic(p);
      break;
    }
    case AT_GPS_DEBUG_ATOMIC: {
      const auto* p = view.atomicAs<gps_debug_atomic_data>(AT_GPS_DEBUG_ATOMIC);
      printGps_debug_atomicAtomic(p);
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