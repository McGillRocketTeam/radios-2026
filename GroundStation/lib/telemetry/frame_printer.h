// packet_printer.h
#pragma once
#include <Arduino.h>
#include "telemetry_packets.h"
#include "frame_view.h"

// Print a single atomic by type (fetches & decodes from FrameView)
void printAtomic(const FrameView &view, AtomicType at);

// Print all present atomics in the view (ordered by enum)
void printAtomics(const FrameView &view);

// Type-specific printers (you can call these directly if you already have the pointer)
void printPropAtomic(const prop_atomic_data *p);
void printValveAtomic(const valve_atomic_data *v);
void printTestAtomic(const test_atomic_data *t);
void printRecovAtomic(const recov_atomic_data *r);
void printFlightAtomic(const flight_atomic_data *f);

// Optional: pretty name for types
const __FlashStringHelper *atomicTypeName(AtomicType at);
