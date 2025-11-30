#pragma once

#include <Arduino.h>
#include "LoraParamConfig.h"
#include "LoggerConfig.h"


//This is a special debug outside of the logger specific to the console. 
constexpr bool CONSOLE_ROOT_DEBUG = true; 
constexpr int GS_SERIAL_BAUD_RATE = 9600;

// Controls whether this code is allowed to transmit
constexpr bool ENABLE_RADIO_TX = true;

// Radio identity and keywording
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";
constexpr const char *RADIO_COMMAND_KEYWORD = "radio";



