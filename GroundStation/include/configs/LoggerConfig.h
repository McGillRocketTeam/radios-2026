#pragma once
#include <Arduino.h>

// --- Severity ---
enum LogLevel : uint8_t {
  PIPE = 0, // Manual debugging stuff
  DEBUG = 1,
  INFO  = 2,
  CRIT  = 3
};

constexpr LogLevel GS_LOG_LEVEL = DEBUG;

// --- Categories ---
enum LogCat : uint32_t {
  CAT_NONE   = 0,
  CAT_SERIAL = 1u << 0, // serial-only
  CAT_PARSER = 1u << 1,
  CAT_RADIO  = 1u << 2,
  CAT_MQTT   = 1u << 3,
  CAT_GS    = 1u << 4,
  CAT_RANGETEST = 1u << 5,
  CAT_ASTRA_DEBUG = 1u << 6,
  CAT_ALL    = 0xFFFFFFFFu,
};

constexpr uint32_t GS_LOG_CATS = CAT_ASTRA_DEBUG;
