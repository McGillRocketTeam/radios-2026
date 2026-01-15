#pragma once

#include <Arduino.h>

// Log Levels
enum LogLevel : uint8_t {
    PIPE    = 0,
    DEBUG   = 1,    // Debugging information
    INFO    = 2,    // General information
    CRIT    = 3,    // Critical logs
    SPED    = 4    // Extremely special level reserved for testing
};

constexpr LogLevel GS_LOG_LEVEL = LogLevel::SPED;