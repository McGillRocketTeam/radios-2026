#include <Arduino.h>
#include "frame_builder.h"
#include "frame_header.h"
#include "telemetry_packets.h"
#include "frame_view.h"
#include "frame_printer.h"

static void printHex(const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (i && (i % 16 == 0)) Serial.println();
    if (data[i] < 16) Serial.print('0');
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  const uint32_t start = millis();
  while (!Serial && (millis() - start < 3000)) { /* wait */ }

  Serial.println(F("=== Building telemetry frame ==="));

  prop_atomic_data prop{};
  prop.cc_pressure  = 1234;
  prop.tank_pressure= 2310;
  prop.tank_temp    = 875;
  prop.vent_temp    = 200;
  prop.mov_hall_state = true;

  valve_atomic_data valve{};
  valve.fdov_armed_SW            = true;
  valve.fdov_armed_HW            = true;
  valve.fdov_energized_SW        = false;
  valve.fdov_energizedGate_HW    = false;
  valve.fdov_energizedCurrent_HW = true;
  valve.fdov_continuity_HW       = true;
  valve.mov_armed_SW             = true;
  valve.mov_armed_HW             = true;
  valve.mov_energized_SW         = false;
  valve.mov_energizedGate_HW     = false;
  valve.mov_energizedCurrent_HW  = true;
  valve.mov_continuity_HW        = true;

  uint8_t frameBuf[256] = {0};

  FrameBuilder fb(frameBuf, sizeof(frameBuf));

  bool ok = true;
  ok &= fb.addAtomic((int)AT_PROP_ATOMIC,  &prop,  sizeof(prop));
  ok &= fb.addAtomic((int)AT_VALVE_ATOMIC, &valve, sizeof(valve));

  if (!ok) {
    Serial.println(F("Error: atomic size mismatch or buffer too small."));
    return;
  }

  const uint16_t seq    = 1;
  const uint8_t  flags  = FLAG_CTS;
  const uint8_t  ack_id = 0;
  size_t frameLen = fb.finalize(seq, flags, ack_id);

  Serial.print(F("Frame length: "));
  Serial.println(frameLen);
  Serial.println(F("Frame bytes (hex):"));
  printHex(frameBuf, frameLen);

  FrameView view(frameBuf, frameLen);
  auto err = view.validate();
  if (err != ParseError::Ok) {
    Serial.print(F("Validate error: "));
    Serial.println((int)err);
    return;
  }

  const FrameHeader* h = view.header();
  Serial.println(F("\n--- Header ---"));
  Serial.print(F("seq: "));   Serial.println(h->seq);
  Serial.print(F("flags: 0x")); Serial.println(h->flags, HEX);
  Serial.print(F("ack_id: "));  Serial.println(h->ack_id);
  Serial.print(F("bitmap: 0x")); Serial.println(h->atomics_bitmap, HEX);

  Serial.println(F("\n--- Atomics present ---"));
  for (int i = 0; i < AT_TOTAL; ++i) {
    if (view.hasAtomic(i)) {
      Serial.print(F(" - AT index ")); Serial.print(i);
      Serial.print(F(", size ")); Serial.println(AT_SIZE[i]);
    }
  }

  printAtomics( view );
}

void loop() {}
