#pragma once

#include <Arduino.h>
#include "LoraParamConfig.h"


//This is a special debug outside of the logger specific to the console. 
constexpr int GS_SERIAL_BAUD_RATE = 115200;

// Controls whether this code is allowed to transmit
constexpr bool ENABLE_RADIO_TX = true;

// Call Sign
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";



