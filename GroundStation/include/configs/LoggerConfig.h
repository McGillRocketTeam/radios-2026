#pragma once
#include <Arduino.h>
#include "Config.h"

// --- Severity ---
enum LogLevel : uint8_t {
  PIPE = 0, // Manual debugging stuff
  DEBUG = 1,
  INFO  = 2,
  CRIT  = 3
};

constexpr LogLevel GS_LOG_LEVEL = INFO;

// --- Categories ---
enum LoggingCategory : uint32_t {
  CAT_NONE   = 0,
  CAT_SERIAL = 1u << 0, // serial-only
  CAT_PARSER = 1u << 1,
  CAT_RADIO  = 1u << 2,
  CAT_GS    = 1u << 3,
  CAT_RANGETEST = 1u << 4,
  CAT_TELEMETRY = 1u << 5,
  CAT_ALL    = 0xFFFFFFFFu,
};

constexpr uint32_t DEFAULT_ASTRA_PRINT = ENABLE_VERBOSE_PRINT ? CAT_TELEMETRY : 0u;
constexpr uint32_t GS_LOG_CATS = DEFAULT_ASTRA_PRINT | CAT_GS | CAT_RADIO;
