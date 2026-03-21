#pragma once

#include <Arduino.h>
#include "LoraParamConfig.h"

//This is a special debug outside of the logger specific to the console. 
constexpr int GS_SERIAL_BAUD_RATE = 115200;

// Controls whether this code is allowed to transmit
constexpr bool ENABLE_RADIO_TX = true;

// Controls whether we print the contents of the ASTRA packets to serial by default
constexpr bool ENABLE_VERBOSE_PRINT = false;

// Radio identity and keywording
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";
constexpr char RADIO_COMMAND_KEYWORD[] = "radio";
constexpr size_t RADIO_COMMAND_KEY_LEN = sizeof(RADIO_COMMAND_KEYWORD) - 1;

