#include <Arduino.h>
#include "telemetry_packets.h"
#include "frame_view.h"

static void printPropAtomic(const prop_atomic_data* p) {
  if (!p) return;
  Serial.println(F("prop_atomic {"));
  Serial.print(F("  cc_pressure: "));   Serial.println(p->cc_pressure);
  Serial.print(F("  tank_pressure: ")); Serial.println(p->tank_pressure);
  Serial.print(F("  tank_temp: "));     Serial.println(p->tank_temp);
  Serial.print(F("  vent_temp: "));     Serial.println(p->vent_temp);
  Serial.print(F("  mov_hall_state: "));Serial.println(p->mov_hall_state ? F("true") : F("false"));
  Serial.println(F("}"));
}

static void printValveAtomic(const valve_atomic_data* v) {
  if (!v) return;
  Serial.println(F("valve_atomic {"));
  Serial.print(F("  fdov_armed_SW: "));            Serial.println(v->fdov_armed_SW ? F("true") : F("false"));
  Serial.print(F("  fdov_armed_HW: "));            Serial.println(v->fdov_armed_HW ? F("true") : F("false"));
  Serial.print(F("  fdov_energized_SW: "));        Serial.println(v->fdov_energized_SW ? F("true") : F("false"));
  Serial.print(F("  fdov_energizedGate_HW: "));    Serial.println(v->fdov_energizedGate_HW ? F("true") : F("false"));
  Serial.print(F("  fdov_energizedCurrent_HW: ")); Serial.println(v->fdov_energizedCurrent_HW ? F("true") : F("false"));
  Serial.print(F("  fdov_continuity_HW: "));       Serial.println(v->fdov_continuity_HW ? F("true") : F("false"));
  Serial.print(F("  mov_armed_SW: "));             Serial.println(v->mov_armed_SW ? F("true") : F("false"));
  Serial.print(F("  mov_armed_HW: "));             Serial.println(v->mov_armed_HW ? F("true") : F("false"));
  Serial.print(F("  mov_energized_SW: "));         Serial.println(v->mov_energized_SW ? F("true") : F("false"));
  Serial.print(F("  mov_energizedGate_HW: "));     Serial.println(v->mov_energizedGate_HW ? F("true") : F("false"));
  Serial.print(F("  mov_energizedCurrent_HW: "));  Serial.println(v->mov_energizedCurrent_HW ? F("true") : F("false"));
  Serial.print(F("  mov_continuity_HW: "));        Serial.println(v->mov_continuity_HW ? F("true") : F("false"));
  Serial.println(F("}"));
}

static void printTestAtomic(const test_atomic_data* t) {
  if (!t) return;
  Serial.println(F("test_atomic {"));
  Serial.print(F("  mov_hall_state: "));     Serial.println(t->mov_hall_state ? F("true") : F("false"));
  Serial.print(F("  fdov_continuity_HW: ")); Serial.println(t->fdov_continuity_HW ? F("true") : F("false"));
  Serial.print(F("  tank_temp: "));          Serial.println(t->tank_temp);
  Serial.println(F("}"));
}

// Main dispatcher over all atomics present in the frame
static void printAtomics(const FrameView& view) {
  Serial.println(F("\n--- Atomics (decoded) ---"));
  if (view.hasAtomic(AT_PROP_ATOMIC)) {
    const auto* p = view.atomicAs<prop_atomic_data>(AT_PROP_ATOMIC);
    printPropAtomic(p);
  }
  if (view.hasAtomic(AT_VALVE_ATOMIC)) {
    const auto* v = view.atomicAs<valve_atomic_data>(AT_VALVE_ATOMIC);
    printValveAtomic(v);
  }
  if (view.hasAtomic(AT_TEST_ATOMIC)) {
    const auto* t = view.atomicAs<test_atomic_data>(AT_TEST_ATOMIC);
    printTestAtomic(t);
  }
}
