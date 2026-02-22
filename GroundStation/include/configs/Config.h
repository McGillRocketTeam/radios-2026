#pragma once

#include <Arduino.h>
#include "LoraParamConfig.h"
#include "LoggerConfig.h"

//This is a special debug outside of the logger specific to the console. 
constexpr int GS_SERIAL_BAUD_RATE = 115200;

// Controls whether this code is allowed to transmit
constexpr bool ENABLE_RADIO_TX = true;

// Controls whether ethernet stuff will be allowed
#if TEENSY == 41
constexpr bool ENABLE_ETHERNET_CONNECTION = true;
#else

// Always false if not on 4.1
constexpr bool ENABLE_ETHERNET_CONNECTION = false;
#endif

// Controls whether human readable telemetry is spit out to serial
constexpr bool ENABLE_VERBOSE_TELMETRY_PACKET = false;

// Radio identity and keywording
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";
constexpr const char *RADIO_COMMAND_KEYWORD = "radio";



