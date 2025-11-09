// packet_printer.cpp
#include "frame_printer.h"

static inline void printBoolLine(const __FlashStringHelper *label, bool b)
{
    Serial.print(label);
    Serial.println(b ? F("true") : F("false"));
}

const __FlashStringHelper *atomicTypeName(AtomicType at)
{
    switch (at)
    {
    case AT_PROP_ATOMIC:
        return F("prop_atomic");
    case AT_VALVE_ATOMIC:
        return F("valve_atomic");
    case AT_TEST_ATOMIC:
        return F("test_atomic");
    case AT_RECOV_ATOMIC:
        return F("recov_atomic");
    case AT_FLIGHT_ATOMIC:
        return F("flight_atomic");
    default:
        return F("unknown_atomic");
    }
}

// ---------------- prop_atomic ----------------
void printPropAtomic(const prop_atomic_data *p)
{
    if (!p)
        return;
    Serial.println(F("prop_atomic {"));
    Serial.print(F("  cc_pressure: "));
    Serial.println(p->cc_pressure);
    Serial.print(F("  tank_pressure: "));
    Serial.println(p->tank_pressure);
    Serial.print(F("  tank_temp: "));
    Serial.println(p->tank_temp);
    Serial.print(F("  vent_temp: "));
    Serial.println(p->vent_temp);
    Serial.print(F("  mov_hall_state: "));
    Serial.println(p->mov_hall_state ? F("true") : F("false"));
    // Keep/extend as your fields evolve:
    Serial.print(F("  prop_energized_electric: "));
    Serial.println(p->prop_energized_electric ? F("true") : F("false"));
    Serial.print(F("  vent_armed_SW: "));
    Serial.println(p->vent_armed_SW ? F("true") : F("false"));
    Serial.print(F("  vent_armed_HW: "));
    Serial.println(p->vent_armed_HW ? F("true") : F("false"));
    Serial.print(F("  vent_energized_SW: "));
    Serial.println(p->vent_energized_SW ? F("true") : F("false"));
    Serial.print(F("  vent_energizedGate_HW: "));
    Serial.println(p->vent_energizedGate_HW ? F("true") : F("false"));
    Serial.print(F("  vent_energizedCurrent_HW: "));
    Serial.println(p->vent_energizedCurrent_HW ? F("true") : F("false"));
    Serial.print(F("  vent_continuity_HW: "));
    Serial.println(p->vent_continuity_HW ? F("true") : F("false"));
    Serial.println(F("}"));
}

// ---------------- valve_atomic ----------------
void printValveAtomic(const valve_atomic_data *v)
{
    if (!v)
        return;
    Serial.println(F("valve_atomic {"));
    printBoolLine(F("  fdov_armed_SW: "), v->fdov_armed_SW);
    printBoolLine(F("  fdov_armed_HW: "), v->fdov_armed_HW);
    printBoolLine(F("  fdov_energized_SW: "), v->fdov_energized_SW);
    printBoolLine(F("  fdov_energizedGate_HW: "), v->fdov_energizedGate_HW);
    printBoolLine(F("  fdov_energizedCurrent_HW: "), v->fdov_energizedCurrent_HW);
    printBoolLine(F("  fdov_continuity_HW: "), v->fdov_continuity_HW);
    printBoolLine(F("  mov_armed_SW: "), v->mov_armed_SW);
    printBoolLine(F("  mov_armed_HW: "), v->mov_armed_HW);
    printBoolLine(F("  mov_energized_SW: "), v->mov_energized_SW);
    printBoolLine(F("  mov_energizedGate_HW: "), v->mov_energizedGate_HW);
    printBoolLine(F("  mov_energizedCurrent_HW: "), v->mov_energizedCurrent_HW);
    printBoolLine(F("  mov_continuity_HW: "), v->mov_continuity_HW);
    Serial.println(F("}"));
}

// ---------------- test_atomic ----------------
void printTestAtomic(const test_atomic_data *t)
{
    if (!t)
        return;
    Serial.println(F("test_atomic {"));
    printBoolLine(F("  mov_hall_state: "), t->mov_hall_state);
    printBoolLine(F("  fdov_continuity_HW: "), t->fdov_continuity_HW);
    Serial.print(F("  tank_temp: "));
    Serial.println(t->tank_temp);
    Serial.print(F("  atm_pressure: "));
    Serial.println(t->atm_pressure);
    Serial.print(F("  barometer_altitude: "));
    Serial.println(t->barometer_altitude);
    Serial.print(F("  atm_temp: "));
    Serial.println(t->atm_temp);
    Serial.print(F("  fc_rssi: "));
    Serial.println(t->fc_rssi);
    Serial.print(F("  fc_snr: "));
    Serial.println(t->fc_snr);
    Serial.println(F("}"));
}

// ---------------- recov_atomic ----------------
void printRecovAtomic(const recov_atomic_data *r)
{
    if (!r)
        return;
    Serial.println(F("recov_atomic {"));
    printBoolLine(F("  pilot_armed_SW: "), r->pilot_armed_SW);
    printBoolLine(F("  pilot_armed_HW: "), r->pilot_armed_HW);
    printBoolLine(F("  pilot_energized_SW: "), r->pilot_energized_SW);
    printBoolLine(F("  pilot_energizedGate_HW: "), r->pilot_energizedGate_HW);
    printBoolLine(F("  pilot_energizedCurrent_HW: "), r->pilot_energizedCurrent_HW);
    printBoolLine(F("  pilot_continuity_HW: "), r->pilot_continuity_HW);
    printBoolLine(F("  ring_armed_SW: "), r->ring_armed_SW);
    printBoolLine(F("  ring_armed_HW: "), r->ring_armed_HW);
    printBoolLine(F("  ring_energized_SW: "), r->ring_energized_SW);
    printBoolLine(F("  ring_energizedGate_HW: "), r->ring_energizedGate_HW);
    printBoolLine(F("  ring_energizedCurrent_HW: "), r->ring_energizedCurrent_HW);
    printBoolLine(F("  ring_continuity_HW: "), r->ring_continuity_HW);
    Serial.println(F("}"));
}

// ---------------- flight_atomic ----------------
void printFlightAtomic(const flight_atomic_data *f)
{
    if (!f)
        return;
    Serial.println(F("flight_atomic {"));
    Serial.print(F("  flight_stage: "));
    Serial.println(f->flight_stage);
    Serial.print(F("  altimeter_altitude: "));
    Serial.println(f->altimeter_altitude);
    Serial.print(F("  altitude_from_sea_level: "));
    Serial.println(f->altitude_from_sea_level);
    Serial.print(F("  apogee_from_ground: "));
    Serial.println(f->apogee_from_ground);
    Serial.print(F("  gps_latitude: "));
    Serial.println(f->gps_latitude, 6);
    Serial.print(F("  gps_longitude: "));
    Serial.println(f->gps_longitude, 6);
    Serial.print(F("  gps_altitude: "));
    Serial.println(f->gps_altitude);
    Serial.print(F("  gps_time_last_update: "));
    Serial.println(f->gps_time_last_update);
    Serial.print(F("  vertical_speed: "));
    Serial.println(f->vertical_speed);
    Serial.print(F("  acceleration_x: "));
    Serial.println(f->acceleration_x);
    Serial.print(F("  acceleration_y: "));
    Serial.println(f->acceleration_y);
    Serial.print(F("  acceleration_z: "));
    Serial.println(f->acceleration_z);
    Serial.print(F("  angle_yaw: "));
    Serial.println(f->angle_yaw);
    Serial.print(F("  angle_pitch: "));
    Serial.println(f->angle_pitch);
    Serial.print(F("  angle_roll: "));
    Serial.println(f->angle_roll);
    Serial.println(F("}"));
}

// ---------------- dispatcher ----------------
void printAtomic(const FrameView &view, AtomicType at)
{
    switch (at)
    {
    case AT_PROP_ATOMIC:
    {
        const auto *p = view.atomicAs<prop_atomic_data>(AT_PROP_ATOMIC);
        printPropAtomic(p);
        break;
    }
    case AT_VALVE_ATOMIC:
    {
        const auto *v = view.atomicAs<valve_atomic_data>(AT_VALVE_ATOMIC);
        printValveAtomic(v);
        break;
    }
    case AT_TEST_ATOMIC:
    {
        const auto *t = view.atomicAs<test_atomic_data>(AT_TEST_ATOMIC);
        printTestAtomic(t);
        break;
    }
    case AT_RECOV_ATOMIC:
    {
        const auto *r = view.atomicAs<recov_atomic_data>(AT_RECOV_ATOMIC);
        printRecovAtomic(r);
        break;
    }
    case AT_FLIGHT_ATOMIC:
    {
        const auto *f = view.atomicAs<flight_atomic_data>(AT_FLIGHT_ATOMIC);
        printFlightAtomic(f);
        break;
    }
    default:
        Serial.print(F("Unknown atomic type: "));
        Serial.println(static_cast<int>(at));
        break;
    }
}


    void printAtomics(const FrameView &view)
    {
        Serial.println(F("\n--- Atomics (decoded) ---"));
        for (int i = 0; i < AT_TOTAL; ++i)
        {
            auto at = static_cast<AtomicType>(i);
            if (view.hasAtomic(at))
            {
                printAtomic(view, at);
            }
        }
    }

