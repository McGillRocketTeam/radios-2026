// packet_printer.h
#pragma once
#include <Arduino.h>
#include "telemetry_packets.h"
#include "frame_view.h"

// Print a single atomic by type (fetches & decodes from FrameView)
void printAtomic(const FrameView& view, AtomicType at);

// Print all present atomics in the view (ordered by enum)
void printAtomics(const FrameView& view);

const __FlashStringHelper* atomicTypeName(AtomicType at);
